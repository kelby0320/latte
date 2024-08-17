#include "mm/kalloc.h"

#include "errno.h"
#include "libk/memory.h"
#include "libk/print.h"
#include "mm/buddy.h"

#define paddr_to_idx(paddr) (((size_t)paddr - allocators_offset) / BUDDY_BLOCK_MAX_SIZE)

static struct buddy_allocator allocators[MAX_ALLOCATORS] = {0};
static size_t allocators_len = 0;
static size_t allocators_offset = 0;
static size_t contiguous_allocations[MAX_ALLOCATORS] = {0};

void
kalloc_init(void *saddr, size_t mem_size)
{
    printk("Init physical memory allocator starting at %d\n", (int)saddr);
    
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
    size_t buddy_alloc_idx = paddr_to_idx(paddr);
    buddy_allocator_free(&allocators[buddy_alloc_idx], paddr);
}

int
kalloc_link_contiguous_allocation(void *paddr1, void *paddr2)
{
    size_t paddr1_idx = paddr_to_idx(paddr1);
    size_t paddr2_idx = paddr_to_idx(paddr2);

    if (allocators[paddr1_idx].mem_available != 0) {
        return -EINVAL;
    }

    if (allocators[paddr2_idx].mem_available != 0) {
        return -EINVAL;
    }

    contiguous_allocations[paddr1_idx] = paddr2_idx;

    return 0;
}

int
kalloc_unlink_contiguous_allocation(void *paddr1, void *paddr2)
{
    size_t paddr1_idx = paddr_to_idx(paddr1);
    size_t paddr2_idx = paddr_to_idx(paddr2);

    if (contiguous_allocations[paddr1_idx] != paddr2_idx) {
        return -EINVAL;
    }

    contiguous_allocations[paddr1_idx] = 0;

    return 0;
}

void *
kalloc_get_next_contiguous_allocation(void *paddr)
{
    size_t paddr_idx = paddr_to_idx(paddr);
    int next_alloc = contiguous_allocations[paddr_idx];

    if (next_alloc == 0) {
        return NULL;
    }

    return allocators[next_alloc].base_addr;
}

int
kalloc_size_to_order(size_t size)
{
    int order;

    for (order = 0; order <= BUDDY_BLOCK_MAX_ORDER; order++) {
        size_t block_size = BUDDY_BLOCK_MIN_SIZE << order;
        if ((size == block_size) || (size / block_size) == 0) {
            return order;
        }
    }

    return -EINVAL;
}
