#include "proc/process.h"

#include "config.h"
#include "gdt/gdt.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/memory.h"
#include "libk/string.h"
#include "mm/vm.h"
#include "proc/ld.h"
#include "sched/sched.h"

#include <stddef.h>

LIST_ITEM_TYPE_DEF(process_allocation_t);
LIST_ITEM_TYPE_DEF(thread_t);
LIST_ITEM_TYPE_DEF(process_ptr_t);

static LIST(process_ptr_t) process_list = NULL;
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

    res = ld_map_image(&process->elf_img_desc, process->vm_area);
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
        goto err_out1;
    }

    res = vm_area_user_init(process->vm_area);
    if (res < 0) {
        goto err_out2;
    }

    res = process_load_exec(process, filename);
    if (res < 0) {
        goto err_out3;
    }

    int tid = thread_create(process);
    if (tid < 0) {
        goto err_out3;
    }

    struct thread *thread = thread_get(tid);

    res = process_add_thread(process, thread);
    if (res < 0) {
        goto err_out4;
    }

    list_item_append(process_ptr_t, &process_list, (process_ptr_t)process);

    return process->pid;

err_out4:
    thread_destroy(thread);

err_out3:
    process_destroy(process);

err_out2:
    kfree(process->vm_area);

err_out1:
    kfree(process);
    return res;
}

int
process_destroy(struct process *process)
{
    // TODO

    return 0;
}

void
process_switch_to_vm_area(struct process *process)
{
    gdt_set_user_data_segment();
    vm_area_switch_map(process->vm_area);
}

int
process_add_thread(struct process *process, struct thread *thread)
{
    list_item_append(thread_t, &process->threads, (thread_t)*thread);

    sched_add_thread(thread);

    return 0;
}