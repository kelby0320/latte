#ifndef ALLOC_H
#define ALLOC_H

#include "mm/buddy.h"

#include <stddef.h>

#define MAX_ALLOCATABLE_MEMORY 0x100000000 // 4GB
#define MAX_ALLOCATORS         (MAX_ALLOCATABLE_MEMORY / BUDDY_BLOCK_MAX_SIZE)
#define PHYS_PAGE_SIZE         4096
#define PHYS_PAGE_ORDER        0

#define kalloc_order_to_size(order)  (BUDDY_BLOCK_MIN_SIZE * (1 << order))
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
 * @brief Link two contiguous physical memory allocations
 *
 * @param paddr1    Physical address of the first allocation
 * @param paddr2    Physical address of the second allocation
 * @return int
 */
int
kalloc_link_contiguous_allocation(void *paddr1, void *paddr2);

/**
 * @brief   Unlink two contiguous physical memory allocations
 *
 * @param paddr1    Physical address of the first allocation
 * @param paddr2    Physical address of the second allocation
 * @return int
 */
int
kalloc_unlink_contiguous_allocation(void *paddr1, void *paddr2);

/**
 * @brief   Get the next contiguous allocation
 * 
 * @param paddr     Physical address of the current allocation
 * @return void*    Physical address of the next allocation
 */
void *
kalloc_get_next_contiguous_allocation(void *paddr);

/**
 * @brief   Get the order of a given size
 * 
 * @param size  Size of the allocation
 * @return int  Order of the allocation
 */
int
kalloc_size_to_order(size_t size);


#endif