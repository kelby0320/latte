#include "libk/alloc.h"

static void *kernel_heap_vaddr_start = NULL;
static void *kernel_heap_vaddr_top = NULL;

void
libk_alloc_init(void *saddr)
{
    kernel_heap_vaddr_start = saddr;
    kernel_heap_vaddr_top = saddr;
}

void *
vmalloc(size_t size);

void *
vzalloc(size_t size);

void
vfree();

void *
kmalloc(size_t size);

void *
kzalloc(size_t size);

void
kfree(void *ptr);