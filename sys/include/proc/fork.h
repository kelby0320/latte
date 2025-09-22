#ifndef PROC_MGMT_FORK_H
#define PROC_MGMT_FORK_H

struct process;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief   Fork a new process.
 *
 * @param parent    The parent process.
 * @param child     Pointer to the child process.
 * @return int      Status code
 */
int
process_fork(struct process *parent, struct process **child);

#ifdef __cplusplus
}
#endif
#endif
