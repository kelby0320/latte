#include "unistd.h"

#include "syscall.h"

int
close(int fd)
{
    return do_syscall1(CLOSE_SYSCALL_NO, fd);
}