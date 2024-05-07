#include "unistd.h"

#include "syscall.h"

void
_exit(int status)
{
    do_syscall1(EXIT_SYSCALL_NO, (uint32_t)status);
}