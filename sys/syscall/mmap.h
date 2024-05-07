#ifndef SYSCALL_MMAP_H
#define SYSCALL_MMAP_H

struct thread;

/**
 * @brief Execute the mmap system call
 *
 */
void
do_mmap(struct thread *current_thread);

/**
 * @brief Execute the munmap system call
 *
 */
void
do_munmap(struct thread *current_thread);

#endif