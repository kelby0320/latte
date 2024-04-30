#include "stdlib.h"

#include "sys/mmap.h"

void *
malloc(size_t size)
{
    return mmap(size);
}