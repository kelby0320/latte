#ifndef PROC_MGMT_EXEC_H
#define PROC_MGMT_EXEC_H

struct process;

/**
 * @brief   Execute a new program.
 *
 * @param process   The process to execute the program in.
 * @param argv      The arguments to the program.
 * @param envp      The environment variables.
 * @return int      Status code
 */
int
process_execve(struct process *process, const char *const *argv, const char *const *envp);

#endif