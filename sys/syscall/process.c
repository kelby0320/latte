#include "syscall/process.h"

#include "proc/process.h"
#include "sched/sched.h"
#include "thread/userio.h"

void *
do_fork()
{
    return 0;
}

void *
do_execve()
{
    return 0;
}

void *
do_wait()
{
    return 0;
}

void *
do_exit()
{
    struct thread *current_thread = sched_get_current();
    int status_code = (int)thread_get_stack_item(current_thread, 0);

    process_terminate(current_thread->process, (uint8_t)status_code);

    schedule();

    // No return because of schedule()
    return NULL;
}
