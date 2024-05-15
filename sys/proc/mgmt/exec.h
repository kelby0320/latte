#ifndef PROC_MGMT_EXEC_H
#define PROC_MGMT_EXEC_H

struct process;

/**
 * @brief   Execute a new program.
 *
 * @param process   The process to execute the program in
 * @param argv      The arguments to the program
 * @param path      The path to the program
 * @return int      Status code
 */
int
process_execv(struct process *process, const char *path, const char *const *argv);

#endif