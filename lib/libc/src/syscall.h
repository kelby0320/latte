#ifndef LIBC_SYSCALL_H
#define LIBC_SYSCALL_H

#define OPEN_SYSCALL_NO   0
#define CLOSE_SYSCALL_NO  1
#define READ_SYSCALL_NO   2
#define WRITE_SYSCALL_NO  3
#define MMAP_SYSCALL_NO   4
#define MUNMAP_SYSCALL_NO 5
#define FORK_SYSCALL_NO   6
#define EXECVE_SYSCALL_NO 7
#define WAIT_SYSCALL_NO   8
#define EXIT_SYSCALL_NO   9

int
do_syscall1(int syscall_no, int arg1);

int
do_syscall2(int syscall_no, int arg1, int arg2);

int
do_syscall3(int syscall_no, int arg1, int arg2, int arg3);

#endif