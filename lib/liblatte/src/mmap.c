#include "latte/mmap.h"

extern void *
latte_mmap(size_t size);

extern int
latte_munmap(void *ptr);

void *
mmap(size_t size)
{
    return latte_mmap(size);
}

int
munmap(void *ptr)
{
    return latte_munmap(ptr);
}
