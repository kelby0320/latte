#include "proc/wait.h"

#include "errno.h"
#include "libk/list.h"
#include "process.h"

pid_t
process_wait(struct process *process, int *status)
{
    for_each_in_list(struct process *, process->children, list, child)
    {
        if (child->state == PROCESS_STATE_ZOMBIE) {
            *status = child->status_code;
            pid_t child_pid = child->pid;
            process_remove(child);
            return child_pid;
        }
    }

    return -ECHILD;
}
