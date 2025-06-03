#ifndef PAGING_H
#define PAGING_H

#include "paging/page_dir.h"
#include "paging/page_tbl.h"

#include <stdbool.h>
#include <stddef.h>

#define PAGING_PAGE_PRESENT        0b00000001
#define PAGING_PAGE_WRITABLE       0b00000010
#define PAGING_PAGE_USER           0b00000100
#define PAGING_PAGE_WRITE_THROUGH  0b00001000
#define PAGING_PAGE_CACHE_DISABLED 0b00010000
#define PAGING_PAGE_ACCESSED       0b00100000
#define PAGING_PAGE_DIRTY          0b01000000

#define PAGING_PAGE_DIR_MASK    0xFFC00000
#define PAGING_PAGE_TBL_MASK    0x003FF000
#define PAGING_PAGE_OFFSET_MASK 0xFFFFF000

#define PAGING_PAGE_SIZE          4096
#define PAGING_PAGE_TBL_ENTRIES   1024
#define PAGING_PAGE_DIR_ENTRIES   1024
#define PAGING_LARGE_PAGE_SIZE    (PAGING_PAGE_SIZE * PAGING_PAGE_TBL_ENTRIES)
#define PAGING_KERNEL_DIR_ENTRIES 256
#define PAGING_KERNEL_DIR_OFFSET  768

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
 * @brief   Copy kernel page directory entries to user page directory
 *
 * Copied entries are marked as supervisor and so are still inaccessable by user
 * mode code
 *
 * @param kernel_page_dir   Kernel page directory
 * @param user_page_dir     User page directory
 */
void
paging_copy_kernel_pages_to_user(
    page_dir_t kernel_page_dir, page_dir_t user_page_dir);

/**
 * @brief   Find a free page in the page directory
 *
 * @param page_dir          The page directory
 * @param base_vaddr        The virtual address to start the search from
 * @return void*            The address of the free page
 */
void *
paging_find_free_page(page_dir_t page_dir, void *base_vaddr);

/**
 * @brief   Find a free extent of pages in the page directory
 *
 * @param page_dir          The page directory
 * @param base_vaddr        The virtual address to start the search from
 * @param size              The number of pages to find
 * @return void*            The address of the free extent
 */
void *
paging_find_free_extent(
    page_dir_t page_dir, void *base_vaddr, size_t num_pages);

#endif
