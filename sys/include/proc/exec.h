#ifndef PROC_MGMT_EXEC_H
#define PROC_MGMT_EXEC_H

#include <stddef.h>

struct process;

/**
 * @brief   Execute a new program.
 *
 * @param process   The process to execute the program in
 * @param path      The path to the program
 * @param argv      The arguments to the program
 * @param argv_len  The length of the arguments array
 * @return int      Status code
 */
int
process_execv(
    struct process *process, const char *path, const char *const *argv,
    size_t argv_len);

#endif
