#include "syscall/syscall.h"

#include "config.h"
#include "errno.h"
#include "irq/isr.h"
#include "syscall/io.h"
#include "syscall/mmap.h"

typedef void *(*syscall_t)();

static syscall_t syscalls[] = {do_open, do_close, do_read, do_write, do_mmap, do_munmap};

void *
do_syscall(int syscall_no)
{
    if (syscall_no < 0 || syscall_no >= sizeof(syscalls)) {
        return (void *)-EINVAL;
    }

    if (!syscalls[syscall_no]) {
        return (void *)-EINVAL;
    }

    return syscalls[syscall_no]();
}