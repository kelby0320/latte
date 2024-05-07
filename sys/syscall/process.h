#ifndef SYSCALL_PROCESS_H
#define SYSCALL_PROCESS_H

struct thread;

/**
 * @brief   Execute the fork system call
 *
 */
void
do_fork(struct thread *current_thread);

/**
 * @brief   Execute the execve system call
 *
 */
void
do_execve(struct thread *current_thread);

/**
 * @brief   Execute the wait system call
 *
 */
void
do_wait(struct thread *current_thread);

/**
 * @brief   Execute the exit system call
 *
 */
void
do_exit(struct thread *current_thread);

#endif