#include "mm/kalloc.h"

#include "libk/memory.h"
#include "mm/buddy.h"

static struct buddy_allocator allocators[MAX_ALLOCATORS] = {0};
static size_t allocators_len = 0;
static size_t allocators_offset = 0;

void
kalloc_init(void *saddr, size_t mem_size)
{
    allocators_offset = (size_t)saddr;
    allocators_len = (mem_size - allocators_offset) / BUDDY_BLOCK_MAX_SIZE;

    for (size_t i = 0; i < allocators_len; i++) {
        buddy_allocator_init(&allocators[i], saddr + (i * BUDDY_BLOCK_MAX_SIZE));
    }
}

void *
kalloc_get_phys_pages(size_t order)
{
    for (size_t i = 0; i < allocators_len; i++) {
        if (allocators[i].mem_available >= PHYS_PAGE_SIZE) {
            return buddy_allocator_malloc(&allocators[i], order);
        }
    }

    return NULL;
}

void
kalloc_free_phys_pages(void *paddr)
{
    size_t buddy_alloc_idx = ((size_t)paddr - allocators_offset) / BUDDY_BLOCK_MAX_SIZE;
    buddy_allocator_free(&allocators[buddy_alloc_idx], paddr);
}