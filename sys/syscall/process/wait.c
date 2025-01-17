#include "syscall/process.h"

#include "proc/wait.h"
#include "process.h"
#include "sched.h"
#include "thread.h"
#include "userio.h"

void
do_wait(struct thread *current_thread)
{
    int *user_status = (int *)thread_get_stack_item(current_thread, 0);

    int status = -1;
    pid_t child_pid = process_wait(current_thread->process, &status);

    thread_set_return_value(current_thread, child_pid);

    if ((int)child_pid < 0) {
        sched_block_thread(current_thread);
        return;
    }

    thread_copy_to_user(current_thread, user_status, &status, sizeof(int));
}
