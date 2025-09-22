#ifndef SYSCALL_H
#define SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Execute a system call
 *
 * @param syscall_no    The system call number
 */
void
do_syscall(int syscall_no);

#ifdef __cplusplus
}
#endif
#endif
