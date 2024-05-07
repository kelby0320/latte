#ifndef SYSCALL_PROCESS_H
#define SYSCALL_PROCESS_H

/**
 * @brief   Execute the fork system call
 *
 * @return void*
 */
void *
do_fork();

/**
 * @brief   Execute the execve system call
 *
 * @return void*
 */
void *
do_execve();

/**
 * @brief   Execute the wait system call
 *
 * @return void*
 */
void *
do_wait();

/**
 * @brief   Execute the exit system call
 *
 * @return void*
 */
void *
do_exit();

#endif