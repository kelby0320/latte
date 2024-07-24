#ifndef LIBC_SYSCALL_H
#define LIBC_SYSCALL_H

#define OPEN_SYSCALL_NO         0
#define CLOSE_SYSCALL_NO        1
#define READ_SYSCALL_NO         2
#define WRITE_SYSCALL_NO        3
#define MMAP_SYSCALL_NO         4
#define MUNMAP_SYSCALL_NO       5
#define FORK_SYSCALL_NO         6
#define EXECV_SYSCALL_NO        7
#define WAIT_SYSCALL_NO         8
#define EXIT_SYSCALL_NO         9
#define OPENDIR_SYSCALL_NO      10
#define CLOSEDIR_SYSCALL_NO     11
#define READDIR_SYSCALL_NO      12

int
do_syscall0(int syscall_no);

int
do_syscall1(int syscall_no, int arg1);

int
do_syscall2(int syscall_no, int arg1, int arg2);

int
do_syscall3(int syscall_no, int arg1, int arg2, int arg3);

#endif
