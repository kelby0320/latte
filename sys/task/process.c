#include "task/process.h"

#include "config.h"
#include "errno.h"
#include "gdt/gdt.h"
#include "libk/alloc.h"
#include "libk/memory.h"
#include "libk/string.h"
#include "mm/paging/paging.h"
#include "mm/vm.h"
#include "task/loader.h"
#include "task/sched.h"
#include "task/task.h"

static struct process *pslots[LATTE_PROCESS_MAX_PROCESSES] = {0};

/**
 * @brief Get a free slot for a process
 *
 * @return int Process Id
 */
static int
get_free_slot()
{
    for (int i = 0; i < LATTE_PROCESS_MAX_PROCESSES; i++) {
        if (pslots[i] == 0) {
            return i;
        }
    }

    return -EAGAIN;
}

/**
 * @brief Get a process from a slot
 *
 * @param pid       Process Id
 * @param process   Pointer to the process pointer
 * @return int      Status code
 */
static int
get_process_from_slot(int pid, struct process **process)
{
    if (pid < 0 || pid >= LATTE_PROCESS_MAX_PROCESSES) {
        return -EINVAL;
    }

    *process = pslots[pid];

    return 0;
}

/**
 * @brief Assign a process to a slot
 *
 * @param pid       Pid of the process
 * @param process   Pointer to the process
 * @return int      Status code
 */
static int
set_slot(int pid, struct process *process)
{
    if (pid < 0 || pid >= LATTE_PROCESS_MAX_PROCESSES) {
        return -EINVAL;
    }

    if (pslots[pid] != 0) {
        return -EAGAIN;
    }

    pslots[pid] = process;
    return 0;
}

static struct process_allocation *
process_get_allocation(struct process *process)
{
    for (int i = 0; i < LATTE_PROCESS_MAX_ALLOCATIONS; i++) {
        struct process_allocation *palloc = &process->allocations[i];
        if (palloc->ptr == NULL && palloc->size == 0) {
            return palloc;
        }
    }

    return NULL;
}

/**
 * @brief Initialize a new process
 *
 * @param process   Pointer to the process
 * @param pid       Process Id
 * @return int      Status code
 */
static int
process_init(struct process *process, int pid)
{
    process->id = pid;
    process->vm_area = kzalloc(sizeof(struct vm_area));

    int res = vm_area_init(process->vm_area);
    if (res < 0) {
        kfree(process->vm_area);
        return res;
    }

    return 0;
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

    int res = loader_init_image(&process->elf_img_desc, filename);
    if (res < 0) {
        goto err_out;
    }

    res = loader_map_image(&process->elf_img_desc, process->vm_area);
    if (res < 0) {
        // If this fails, what is the state of vm_area?
        goto err_out;
    }

    return 0;

err_out:
    loader_free_image(&process->elf_img_desc);
    memset(&process->elf_img_desc, 0, sizeof(process->elf_img_desc));
    memset(&process->filename, 0, sizeof(process->filename));
    return res;
}

int
process_spawn(const char *filename)
{
    int pid = get_free_slot();
    if (pid < 0) {
        return -EAGAIN;
    }

    struct process *process = kzalloc(sizeof(struct process));
    if (!process) {
        return -ENOMEM;
    }

    int res = process_init(process, pid);
    if (res < 0) {
        goto err_out1;
    }

    res = process_load_exec(process, filename);
    if (res < 0) {
        goto err_out2;
    }

    int tid = task_new(process);
    if (tid < 0) {
        goto err_out2;
    }

    struct task *task = task_get(tid);
    if (!task) {
        goto err_out2; // Note: tid is orphaned here, shouldn't happen though
    }

    res = process_add_task(process, task);
    if (res < 0) {
        goto err_out3;
    }

    res = set_slot(pid, process);
    if (res < 0) {
        goto err_out3;
    }

    return pid;

err_out3:
    task_free(task);

err_out2:
    process_terminate(process);

err_out1:
    kfree(process);
    return res;
}

int
process_terminate(struct process *process)
{
    // TODO
    return 0;
}

int
process_add_task(struct process *process, struct task *task)
{
    if (process->num_tasks == LATTE_PROCESS_MAX_TASKS) {
        return -EAGAIN;
    }

    process->tasks[process->num_tasks] = task;
    process->num_tasks++;

    sched_add_task(task);

    return 0;
}

int
process_remove_task(struct process *process, uint16_t task_id)
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

void *
process_mmap(struct process *process, size_t size)
{
    if (size % PAGING_PAGE_SIZE) {
        size = (size + PAGING_PAGE_SIZE) - (size % PAGING_PAGE_SIZE);
    }

    struct process_allocation *palloc = process_get_allocation(process);
    if (!palloc) {
        goto err_out;
    }

    void *ptr = kzalloc(size);
    if (!ptr) {
        goto err_out;
    }

    int res = vm_area_map_pages_to(process->vm_area, ptr, ptr, ptr + size,
                                   PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER);
    if (res < 0) {
        kfree(ptr);
        goto err_out;
    }

    palloc->ptr = ptr;
    palloc->size = size;

    return ptr;

err_out:
    return NULL;
}

void
process_munmap(struct process *process, void *ptr)
{
    // TODO
}
