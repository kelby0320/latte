#include "unistd.h"

#include "syscall.h"

pid_t
fork()
{
    return do_syscall0(FORK_SYSCALL_NO);
}