#include "unistd.h"

#include "syscall.h"

int
execv(const char *path, char *const *argv)
{
    return do_syscall2(EXECV_SYSCALL_NO, (int)path, (int)argv);
}