#ifndef PAGE_TBL_H
#define PAGE_TBL_H

#include <stdint.h>

typedef uint32_t *page_tbl_t;
typedef uint32_t page_tbl_entry_t;

#define page_tbl_from_page_dir_entry(page_dir_entry) ((page_tbl_t)(page_dir_entry & 0xfffff000))
#define page_tbl_starting_addr(page_tbl)             ((void *)((uint32_t)page_tbl & 0xfffff000))

/**
 * @brief Retrieve a page table entry from a page table
 *
 * @param page_tbl          The page table
 * @param vaddr             The virtual address of the entry
 * @return page_tbl_entry_t The page table entry
 */
page_tbl_entry_t
page_tbl_get_entry(page_tbl_t page_tbl, void *vaddr);

/**
 * @brief Set a page table entry on a page table
 *
 * @param page_tbl          The page table
 * @param vaddr             The virtual address of the entry
 * @param page_tbl_entry    The page table entry
 */
void
page_tbl_set_entry(page_tbl_t page_tbl, void *vaddr, page_tbl_entry_t page_tbl_entry);

#endif