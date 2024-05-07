#ifndef PROC_FD_H
#define PROC_FD_H

struct process;

/**
 * @brief   Add a global file descriptor to the process
 *
 * @param process   Pointer to the process
 * @param gfd       Global file descriptor
 * @return int      Process file descriptor
 */
int
process_add_gfd(struct process *process, int gfd);

/**
 * @brief   Get the global file descriptor from a process file descriptor
 *
 * @param process   Pointer to the process
 * @param pfd       Process file descriptor
 * @return int      Global file descriptor
 */
int
process_get_gfd(struct process *process, int pfd);

/**
 * @brief   Remove a process file desciptor
 *
 * @param process   Pointer to the process
 * @param pfd       Process file descriptor
 * @return int      Status code
 */
int
process_remove_pfd(struct process *process, int pfd);

#endif