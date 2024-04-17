#include "proc/mgmt.h"

#include "proc/process.h"

int
process_fork(struct process *process)
{
    // TODO
    return -1;
}

int
process_wait(struct process *process)
{
    // TODO
    return -1;
}

int
process_execve(struct process *process, const char *const *argv, const char *const *envp)
{
    // TODO
    return -1;
}

uint8_t
process_exit(struct process *process, uint8_t status_code)
{
    // TODO
    return -1;
}