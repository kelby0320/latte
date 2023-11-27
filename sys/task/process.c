#include "task/process.h"

#include "config.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/string.h"
#include "mem/vm.h"
#include "task/task.h"

static struct process* pslots[LATTE_PROCESS_MAX_PROCESSES];

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

static int
set_slot(int pid, struct process *process)
{
    if (pslots[pid] != 0) {
        return -EAGAIN;
    }

    pslots[pid] = process;
    return 0;
}

static int
process_init(struct process *process, int pid, const char *filename)
{
    process->id = pid;
    
    strncpy(process->filename, filename, LATTE_MAX_PATH_LEN);
    process->vm_area = kzalloc(sizeof(struct vm_area));

    int res = vm_area_init(process->vm_area, VM_PAGE_PRESENT | VM_PAGE_WRITABLE);
    if (res < 0) {
        kfree(process->vm_area);
        return res;
    }

    return 0;
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

    int res = process_init(process, pid, filename);
    if (res < 0) {
        goto err_out1;
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
    process_free(process);

err_out1:
    kfree(process);
    return res;
}

int
process_add_task(struct process *process, struct task *task)
{
    if (process->num_tasks == LATTE_PROCESS_MAX_TASKS) {
        return -EAGAIN;
    }

    process->tasks[process->num_tasks] = task;
    process->num_tasks++;
}

int
process_remove_task(struct process *process, uint16_t task_id)
{
    return 0;
}

int
process_load_exec(struct process *process)
{
    return 0;
}

void
process_free(struct process *process)
{
    
}