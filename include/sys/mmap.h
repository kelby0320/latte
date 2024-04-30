#ifndef LIBC_SYS_MMAP_H
#define LIBC_SYS_MMAP_H

#include <stddef.h>

void *
mmap(size_t size);

int
munmap(void *ptr);

#endif