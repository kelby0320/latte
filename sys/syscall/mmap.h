#ifndef SYSCALL_MMAP_H
#define SYSCALL_MMAP_H

/**
 * @brief Execute the mmap system call
 * 
 * @return void* Pointer to mapped memory
 */
void *
do_mmap();

/**
 * @brief Execute the munmap system call
 * 
 * @return void* NULL
 */
void *
do_munmap();

#endif