#ifndef PAGE_DIR_H
#define PAGE_DIR_H

#include <stdint.h>

typedef uint32_t *page_dir_t;
typedef uint32_t page_dir_entry_t;

/**
 * @brief Initialize a page directory
 *
 * @param page_dir  The page directory
 */
void
page_dir_init(page_dir_t page_dir);

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
 * @brief Create a new page directory entry for the given virtual address
 *
 * @param page_dir  The page directory
 * @param vaddr     The virtual address
 * @return int      Status code
 */
int
page_dir_create_new_entry(page_dir_t page_dir, void *vaddr);

#endif