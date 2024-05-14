#ifndef PROC_MGMT_WAIT_H
#define PROC_MGMT_WAIT_H

struct process;

/**
 * @brief   Wait for a process to exit.
 *
 * @param process   The process to wait for.
 * @return int      The exit status of the process.
 */
int
process_wait(struct process *process);

#endif