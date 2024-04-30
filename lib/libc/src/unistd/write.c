#include "unistd.h"

#include "syscall.h"

int
write(int fd, const char *ptr, size_t count)
{
    return do_syscall3(WRITE_SYSCALL_NO, fd, (int)ptr, count);
}