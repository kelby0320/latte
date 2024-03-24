#ifndef ALLOC_H
#define ALLOC_H

#include "mm/buddy.h"

#include <stddef.h>

#define MAX_ALLOCATABLE_MEMORY 0x100000000 // 4GB
#define MAX_ALLOCATORS         (MAX_ALLOCATABLE_MEMORY / BUDDY_BLOCK_MAX_SIZE)
#define PHYS_PAGE_SIZE         4096
#define PHYS_PAGE_ORDER        0

#define kalloc_get_phys_page() (kalloc_get_phys_pages(PHYS_PAGE_ORDER))

/**
 * @brief Initialize physical memory allocator
 *
 * @param saddr     Start address of the memory region
 * @param mem_size  Size of the memory region
 */
void
kalloc_init(void *saddr, size_t mem_size);

/**
 * @brief Allocate physical pages of a given order
 *
 * @param order     Order of the pages to allocate
 * @return void*    Physical address of the allocated pages
 */
void *
kalloc_get_phys_pages(size_t order);

/**
 * @brief Free physical memory at a given address
 *
 * @param paddr     Physical address of the page to free
 */
void
kalloc_free_phys_pages(void *paddr);

/**
 * @brief
 *
 * @param paddr1
 * @param paddr2
 * @return int
 */
int
kalloc_link_contiguous_allocation(void *paddr1, void *paddr2);

/**
 * @brief
 *
 * @param paddr1
 * @param paddr2
 * @return int
 */
int
kalloc_unlink_contiguous_allocation(void *paddr1, void *paddr2);

void *
kalloc_get_next_contiguous_allocation(void *paddr);

#endif