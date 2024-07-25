#include "syscall/process.h"

#include "proc/mgmt/fork.h"
#include "proc/process.h"
#include "thread/thread.h"

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
