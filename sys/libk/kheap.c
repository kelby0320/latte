#include "kheap.h"

#include "config.h"
#include "kernel.h"
#include "libk/memory.h"

void
kheap_init()
{
    // TODO
}

void *
kmalloc(size_t size)
{
    return NULL;
}

void *
kzalloc(size_t size)
{
    return NULL;
}

void
kfree(void *ptr)
{
    // heap_free(&kernel_heap, ptr);
}