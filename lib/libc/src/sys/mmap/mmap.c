#include "sys/mmap.h"

#include "syscall.h"

void *
mmap(size_t size)
{
    return (void *)do_syscall1(MMAP_SYSCALL_NO, size);
}