#ifndef PROC_MGMT_WAIT_H
#define PROC_MGMT_WAIT_H

#include "proc/process.h"

/**
 * @brief   Wait for a process to exit.
 *
 * @param process   The process to wait for.
 * @param status    The status of the exited process.
 * @return pid_t    The PID of the exited process.
 */
pid_t
process_wait(struct process *process, int *status);

#endif