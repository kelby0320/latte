#include "syscall/process.h"

#include "proc/mgmt.h"
#include "proc/process.h"
#include "sched/sched.h"
#include "thread/userio.h"

void
do_fork(struct thread *current_thread)
{
    process_fork(current_thread->process);

    schedule();
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

    schedule();
}
