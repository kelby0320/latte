#ifndef PAGE_DIR_H
#define PAGE_DIR_H

#include <stdint.h>

#include "paging/page_tbl.h"

typedef uint32_t *page_dir_t;
typedef uint32_t page_dir_entry_t;

#define page_dir_starting_addr(page_dir) ((void *)((uint32_t)page_dir & 0xfffff000))

/**
 * @brief Retrieve a page directory entry from a page directory
 *
 * @param page_dir          The page directory
 * @param vaddr             The virtual address of the entry
 * @return page_dir_entry_t The page directory entry
 */
page_dir_entry_t
page_dir_get_entry(page_dir_t page_dir, void *vaddr);

/**
 * @brief   Set a page directory entry in a page directory
 *
 * @param page_dir          The page directory
 * @param vaddr             The virtual address of the entry
 * @param page_dir_entry    The page directory entry
 */
void
page_dir_set_entry(page_dir_t page_dir, void *vaddr, page_dir_entry_t page_dir_entry);

/**
 * @brief Create a new page directory entry for the given virtual address
 *
 * @param page_dir          The page directory
 * @param vaddr             The virtual address
 * @return page_dir_entry_t The new page directory entry
 */
page_dir_entry_t
page_dir_add_page_tbl(page_dir_t page_dir, void *vaddr, page_tbl_t page_tbl, uint8_t flags);

#endif
