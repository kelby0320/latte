#include "syscall/process.h"

#include "proc/mgmt/exit.h"
#include "proc/mgmt/fork.h"
#include "proc/process.h"
#include "sched/sched.h"
#include "thread/thread.h"
#include "thread/userio.h"

void
do_fork(struct thread *current_thread)
{
    struct process *child;
    int res = process_fork(current_thread->process, &child);
    if (res < 0) {
        thread_set_return_value(current_thread, -1);
        return;
    }

    thread_set_return_value(current_thread, child->pid);
}

void
do_execve(struct thread *current_thread)
{
}

void
do_wait(struct thread *current_thread)
{
}

void
do_exit(struct thread *current_thread)
{
    int status_code = (int)thread_get_stack_item(current_thread, 0);
    process_exit(current_thread->process, (uint8_t)status_code);
}
