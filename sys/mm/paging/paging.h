#ifndef PAGING_H
#define PAGING_H

#include "mm/paging/page_dir.h"

#include <stdbool.h>
#include <stddef.h>

#define PAGING_PAGE_PRESENT        0b00000001
#define PAGING_PAGE_WRITABLE       0b00000010
#define PAGING_PAGE_USER           0b00000100
#define PAGING_PAGE_WRITE_THROUGH  0b00001000
#define PAGING_PAGE_CACHE_DISABLED 0b00010000
#define PAGING_PAGE_ACCESSED       0b00100000
#define PAGING_PAGE_DIRTY          0b01000000

#define PAGING_PAGE_SIZE        4096
#define PAGING_PAGE_TBL_ENTRIES 1024
#define PAGING_PAGE_DIR_ENTRIES 1024
#define PAGING_LARGE_PAGE_SIZE  (PAGING_PAGE_SIZE * PAGING_PAGE_TBL_ENTRIES)

#define is_aligned(addr) (((uint32_t)addr % PAGING_PAGE_SIZE) == 0)

/**
 * @brief Enable paging
 *
 */
void
paging_enable_paging();

/**
 * @brief Load a new page directory
 *
 * @param page_dir  The page directory
 */
void
paging_load_page_directory(page_dir_t page_dir);

/**
 * @brief Flush the TLB
 *
 */
void
paging_flush_tlb();

/**
 * @brief   Find a free page in the page directory
 *
 * @param page_dir          The page directory
 * @param is_kernel_addr    Whether the address is a kernel address
 * @return void*            The address of the free page
 */
void *
paging_find_free_page(page_dir_t page_dir, bool is_kernel_addr);

/**
 * @brief   Find a free extent of pages in the page directory
 *
 * @param page_dir          The page directory
 * @param is_kernel_addr    Whether the address is a kernel address
 * @param size              The number of 4MB large pages to find
 * @return void*            The address of the free extent
 */
void *
paging_find_free_extent(page_dir_t page_dir, bool is_kernel_addr, size_t size);

#endif