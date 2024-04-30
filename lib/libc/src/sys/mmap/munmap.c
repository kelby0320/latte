#include "sys/mmap.h"

#include "syscall.h"

int
munmap(void *ptr)
{
    return do_syscall1(MUNMAP_SYSCALL_NO, (int)ptr);
}
