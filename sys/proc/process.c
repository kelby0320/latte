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
#include "vfs/vfs.h"

#include <stddef.h>

static struct list_item *process_list = NULL;
static uint32_t next_pid = 1;

/**
 * @brief Get the next pid
 *
 * @return uint32_t
 */
static inline uint32_t
get_next_pid()
{
    return next_pid++;
}

/**
 * @brief Load the process's executable
 *
 * @param pid       Process Id
 * @param filename  Filename
 * @return int      Status code
 */
static int
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
process_create_first(const char *filename)
{
    int res = 0;

    struct process *process = kzalloc(sizeof(struct process));
    if (!process) {
        return -ENOMEM;
    }

    process->pid = get_next_pid();
    process->uid = ROOT_UID;
    process->gid = ROOT_GID;
    process->state = PROCESS_STATE_CREATED;
    process->status_code = 0;
    process->parent = NULL;
    process->children = NULL;
    process->allocations = NULL;

    process->vm_area = kzalloc(sizeof(struct vm_area));
    if (!process->vm_area) {
        goto err_alloc;
    }

    res = vm_area_user_init(process->vm_area);
    if (res < 0) {
        goto err_vmarea;
    }

    res = process_load_exec(process, filename);
    if (res < 0) {
        goto err_proc;
    }

    int tid = thread_create(process);
    if (tid < 0) {
        goto err_proc;
    }

    struct thread *thread = thread_get(tid);

    res = list_push_front(&process->threads, thread);
    if (res < 0) {
        goto err_thread;
    }

    list_push_front(&process_list, process);

    return process->pid;

err_thread:
    thread_destroy(thread);

err_proc:
    process_terminate(process, PROCESS_STATUS_CODE_FAILURE);

err_vmarea:
    kfree(process->vm_area);

err_alloc:
    kfree(process);
    return res;
}

int
process_terminate(struct process *process, uint8_t status_code)
{
    // kill/unschedule all threads
    for_each_in_list(struct thread *, process->threads, tlist, thread) { thread_destroy(thread); }

    list_destroy(process->threads);

    // close all open file descriptors
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

    // free all memory allocations
    for_each_in_list(struct process_allocation *, process->allocations, alloc_list, alloc)
    {
        kalloc_free_phys_pages(alloc->addr);
        kfree(alloc);
    }

    list_destroy(process->allocations);

    // set state and status_code entry
    process->state = PROCESS_STATE_ZOMBIE;
    process->status_code = status_code;

    return 0;
}

void
process_switch_to_vm_area(struct process *process)
{
    gdt_set_user_data_segment();
    vm_area_switch_map(process->vm_area);
}