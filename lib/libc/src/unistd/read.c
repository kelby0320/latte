#include "unistd.h"

#include "syscall.h"

int
read(int fd, char *ptr, size_t count)
{
    return do_syscall3(READ_SYSCALL_NO, fd, (int)ptr, count);
}
