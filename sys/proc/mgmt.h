#ifndef PROC_MGMT_H
#define PROC_MGMT_H

#include <stdint.h>

struct process;
struct process_allocation;

int
process_fork(struct process *process);

int
process_wait(struct process *process);

int
process_execve(struct process *process, const char *const *argv, const char *const *envp);

/**
 * @brief   Terminate a running process
 *
 * @param process       Pointer to the process
 * @param status_code   Exit status code
 * @return int          Status code
 */
int
process_exit(struct process *process, uint8_t status_code);

#endif