#ifndef PROC_MGMT_H
#define PROC_MGMT_H

#include "proc/process.h"

int
process_fork(struct process *process);

int
process_wait(struct process *process);

int
process_execve(struct process *process, const char *const *argv, const char *const *envp);

uint8_t
process_exit(struct process *process, uint8_t status_code);

#endif