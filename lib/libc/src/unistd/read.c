#include "unistd.h"

#include "errno.h"
#include "syscall.h"

int
read(int fd, char *ptr, size_t count)
{
    int num_read;

    do {
        num_read = do_syscall3(READ_SYSCALL_NO, fd, (int)ptr, count);
    } while (num_read == -EAGAIN);

    return num_read;
}
