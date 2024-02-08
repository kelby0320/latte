#ifndef SYSCALL_H
#define SYSCALL_H

/**
 * @brief Execute a system call
 * 
 * @param syscall_no    The system call number
 * @return void*        System call depended return value
 */
void *
do_syscall(int syscall_no);

#endif