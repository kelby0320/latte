#include "proc/process.h"

#include "config.h"
#include "errno.h"
#include "gdt/gdt.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/memory.h"
#include "libk/string.h"
#include "mm/kalloc.h"
#include "mm/vm.h"
#include "proc/fd.h"
#include "proc/ld.h"
#include "proc/mgmt/exit.h"
#include "vfs/vfs.h"

#include <stddef.h>

static struct list_item *process_list = NULL;
static uint32_t next_pid = 1;

uint32_t
process_next_pid()
{
    return next_pid++;
}

static int
allocate_empty_args(struct process *process, struct thread *thread)
{
    // Allocate 1 pages for argv
    char **argv_segment = (char **)kalloc_get_phys_pages(0);
    if (!argv_segment) {
        return -ENOMEM;
    }

    // Zero the page
    memset(argv_segment, 0, PROCESS_ARGV_SIZE);

    // Map the page to the process's vm_area
    void *virt = vm_area_map_user_pages(process->vm_area, argv_segment, PROCESS_ARGV_SIZE);
    if (!virt) {
        goto err_segment_alloc;
    }

    // Create a process allocation entry
    struct process_allocation *palloc = kzalloc(sizeof(struct process_allocation));
    if (!palloc) {
        goto err_segment_alloc;
    }

    palloc->type = PROCESS_ALLOCATION_MEMORY_SEGMENT;
    palloc->paddr = argv_segment;
    palloc->vaddr = virt;
    palloc->size = PROCESS_ARGV_SIZE;

    list_push_front(&process->allocations, palloc);

    // Set the ecx/edx register to point to argc and empty argv
    thread->registers.ecx = 0;
    thread->registers.edx = (uint32_t)virt;

    return 0;

err_segment_alloc:
    kalloc_free_phys_pages(argv_segment);
    return -ENOMEM;
}

static int
allocate_empty_environ(struct process *process, struct thread *thread)
{
    // Allocate 1 pages for envp
    char **envp_segment = (char **)kalloc_get_phys_pages(0);
    if (!envp_segment) {
        return -ENOMEM;
    }

    // Zero the page
    memset(envp_segment, 0, PROCESS_ENVP_SIZE);

    // Map the page to the process's vm_area
    void *virt = vm_area_map_user_pages(process->vm_area, envp_segment, PROCESS_ENVP_SIZE);
    if (!virt) {
        goto err_segment_alloc;
    }

    // Create a process allocation entry
    struct process_allocation *palloc = kzalloc(sizeof(struct process_allocation));
    if (!palloc) {
        goto err_segment_alloc;
    }

    palloc->type = PROCESS_ALLOCATION_MEMORY_SEGMENT;
    palloc->paddr = envp_segment;
    palloc->vaddr = virt;
    palloc->size = PROCESS_ENVP_SIZE;

    list_push_front(&process->allocations, palloc);

    // Set the esi register to point to the empty envp
    thread->registers.esi = (uint32_t)virt;

    return 0;

err_segment_alloc:
    kalloc_free_phys_pages(envp_segment);
    return -ENOMEM;
}

int
process_free_threads(struct process *process)
{
    for_each_in_list(struct thread *, process->threads, tlist, thread) { thread_destroy(thread); }

    list_destroy(process->threads);

    process->threads = NULL;

    return 0;
}

int
process_free_allocations(struct process *process)
{
    for_each_in_list(struct process_allocation *, process->allocations, alloc_list, alloc)
    {
        kalloc_free_phys_pages(alloc->paddr);
        kfree(alloc);
    }

    list_destroy(process->allocations);

    process->allocations = NULL;

    return 0;
}

int
process_free_fds(struct process *process)
{
    for_each_in_list(struct process_fd *, process->open_fds, fdlist, proc_fd)
    {
        int gfd = process_get_gfd(process, proc_fd->pfd);
        if (gfd < 0) {
            kfree(proc_fd);
            continue;
        }

        vfs_close(gfd);
        kfree(proc_fd);
    }

    list_destroy(process->open_fds);

    process->open_fds = NULL;

    return 0;
}

int
process_new_vm_area(struct process *process)
{
    if (process->vm_area) {
        kfree(process->vm_area);
    }

    process->vm_area = kzalloc(sizeof(struct vm_area));
    if (!process->vm_area) {
        return -ENOMEM;
    }

    return vm_area_user_init(process->vm_area);
}

int
process_load_exec(struct process *process, const char *filename)
{
    strncpy(process->filename, filename, LATTE_MAX_PATH_LEN);

    int res = ld_init_image(&process->elf_img_desc, filename);
    if (res < 0) {
        goto err_out;
    }

    res = ld_map_image_to_process(process);
    if (res < 0) {
        // If this fails, what is the state of vm_area?
        goto err_out;
    }

    return 0;

err_out:
    ld_free_image(&process->elf_img_desc);
    memset(&process->elf_img_desc, 0, sizeof(process->elf_img_desc));
    memset(&process->filename, 0, sizeof(process->filename));
    return res;
}

int
process_add_thread(struct process *process)
{
    int res = 0;

    int tid = thread_create(process);
    if (tid < 0) {
        res = tid;
        return res;
    }

    struct thread *thread = thread_get(tid);

    res = list_push_front(&process->threads, thread);
    if (res < 0) {
        return res;
    }

    return 0;
}

int
process_create_first(const char *filename)
{
    int res = 0;

    struct process *process = kzalloc(sizeof(struct process));
    if (!process) {
        return -ENOMEM;
    }

    process->pid = process_next_pid();
    process->uid = ROOT_UID;
    process->gid = ROOT_GID;
    process->state = PROCESS_STATE_CREATED;
    process->status_code = 0;
    process->parent = NULL;
    process->children = NULL;
    process->allocations = NULL;

    res = process_new_vm_area(process);
    if (res < 0) {
        goto err_vm_area;
    }

    res = process_load_exec(process, filename);
    if (res < 0) {
        goto err_proc;
    }

    res = process_add_thread(process);
    if (res < 0) {
        goto err_thread;
    }

    struct thread *main_thread = list_front(process->threads);

    res = allocate_empty_args(process, main_thread);
    if (res < 0) {
        goto err_thread;
    }

    res = allocate_empty_environ(process, main_thread);
    if (res < 0) {
        goto err_thread;
    }

    list_push_front(&process_list, process);

    return process->pid;

err_thread:
    process_free_threads(process);

err_proc:
    process_exit(process, PROCESS_STATUS_CODE_FAILURE);

err_vm_area:
    if (process->vm_area) {
        kfree(process->vm_area);
    }

    kfree(process);
    return res;
}

void
process_switch_to_vm_area(struct process *process)
{
    gdt_set_user_data_segment();
    vm_area_switch_map(process->vm_area);
}