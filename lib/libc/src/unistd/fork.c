#include "unistd.h"

#include "syscall.h"

pid_t
fork(void)
{
    return do_syscall0(FORK_SYSCALL_NO);
}