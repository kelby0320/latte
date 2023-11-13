#include "kheap.h"

#include "config.h"
#include "kernel.h"
#include "libk/memory.h"
#include "mem/heap.h"


struct heap kernel_heap;

void
kheap_init()
{
    int res = heap_init(&kernel_heap, (void*)LATTE_HEAP_ADDRESS);
    if (res < 0) {
        panic("Failed to initialize the kernel heap");
    }
}

void*
kmalloc(size_t size)
{
    return heap_malloc(&kernel_heap, size);
}

void*
kzalloc(size_t size)
{
    void *ptr = heap_malloc(&kernel_heap, size);
    if (!ptr) {
        return NULL;
    }

    memset(ptr, 0, size);
    return ptr;
}

void
kfree(void *ptr)
{
    // heap_free(&kernel_heap, ptr);
}