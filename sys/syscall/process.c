#include "syscall/process.h"

#include "libk/alloc.h"
#include "libk/memory.h"
#include "proc/mgmt/exec.h"
#include "proc/mgmt/exit.h"
#include "proc/mgmt/fork.h"
#include "proc/mgmt/wait.h"
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
do_execv(struct thread *current_thread)
{
    const char *const *user_argv = (const char *)thread_get_stack_item(current_thread, 0);
    const char *user_path = (const char *)thread_get_stack_item(current_thread, 1);

    char path[LATTE_MAX_PATH_LEN];
    memset(path, 0, sizeof(path));

    thread_copy_from_user(current_thread, (void *)user_path, path, sizeof(path) - 1);

    process_execv(current_thread->process, path, NULL);
}

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

void
do_exit(struct thread *current_thread)
{
    int status_code = (int)thread_get_stack_item(current_thread, 0);
    process_exit(current_thread->process, (uint8_t)status_code);
}
