#include "stdlib.h"

#include "sys/mmap.h"

void
free(void *ptr)
{
    munmap(ptr);
}