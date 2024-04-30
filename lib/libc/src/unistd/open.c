#include "unistd.h"

#include "syscall.h"

int
open(const char *filename, const char *mode_str)
{
    return do_syscall2(OPEN_SYSCALL_NO, (int)filename, (int)mode_str);
}