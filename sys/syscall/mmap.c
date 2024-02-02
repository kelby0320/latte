#include "syscall/mmap.h"

#include "errno.h"
#include "irq/isr.h"
#include "task/process.h"
#include "task/sched.h"
#include "task/task.h"

#include <stddef.h>

void *
do_mmap()
{
    struct task *current_task = sched_get_current();
    size_t size = (size_t)task_stack_item(current_task, 0);
    return process_mmap(current_task->process, size);
}

void *
do_munmap()
{
    struct task *current_task = sched_get_current();
    void *ptr = task_stack_item(current_task, 0);
    process_munmap(current_task->process, ptr);
    return NULL;
}