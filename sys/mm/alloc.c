#include "mm/alloc.h"

#include "libk/memory.h"
#include "mm/alloc/buddy.h"

static struct buddy_allocator allocators[MAX_ALLOCATORS] = {0};
static size_t allocators_len = 0;

void
alloc_init(void *saddr, size_t mem_size)
{
    allocators_len = (mem_size - (size_t)saddr) / BUDDY_BLOCK_MAX_SIZE;

    for (size_t i = 0; i < allocators_len; i++) {
        buddy_allocator_init(&allocators[i], saddr + (i * BUDDY_BLOCK_MAX_SIZE));
    }
}

void *
alloc_get_phys_page()
{
    for (size_t i = 0; i < allocators_len; i++) {
        if (allocators[i].mem_available >= PHYS_PAGE_SIZE) {
            return buddy_allocator_malloc(&allocators[i], PHYS_PAGE_ORDER);
        }
    }

    return NULL;
}

void
alloc_free_phys_page(void *paddr)
{
}