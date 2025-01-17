#ifndef SYSCALL_IO_H
#define SYSCALL_IO_H

struct thread;

/**
 * @brief Execute the open system call
 *
 */
void
do_open(struct thread *current_thread);

/**
 * @brief Execute the close system call
 *
 */
void
do_close(struct thread *current_thread);

/**
 * @brief Execute the read system call
 *
 */
void
do_read(struct thread *current_thread);

/**
 * @brief Execute the write system call
 *
 */
void
do_write(struct thread *current_thread);

/**
 * @brief Execute the opendir system call
 */   
void
do_opendir(struct thread *current_thread);

/**
 * @brief Execute the closedir system call
 */
void
do_closedir(struct thread *current_thread);

/**
 * @brief Execute the readdir system call
 */
void
do_readdir(struct thread *current_thread);

#endif
