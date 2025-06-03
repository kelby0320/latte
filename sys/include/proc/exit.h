#ifndef PROC_MGMT_EXIT_H
#define PROC_MGMT_EXIT_H

#include <stdint.h>

struct process;

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