#ifndef PAGING_H
#define PAGING_H

#include "mm/paging/page_dir.h"

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

#define is_aligned(addr) (((uint32_t)addr % PAGING_PAGE_SIZE) == 0)

/**
 * @brief Enable paging
 *
 */
void
enable_paging();

/**
 * @brief Load a new page directory
 *
 * @param page_dir  The page directory
 */
void
load_page_directory(page_dir_t page_dir);

/**
 * @brief Flush the TLB
 * 
 */
void
flush_tlb();

#endif