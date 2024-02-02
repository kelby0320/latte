#ifndef LATTE_MMAP_H
#define LATTE_MMAP_H

#include <stddef.h>

void *
mmap(size_t size);

int
munmap(void *ptr);

#endif
