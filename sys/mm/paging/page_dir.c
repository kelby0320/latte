#include "mm/paging/page_dir.h"

#include "errno.h"
#include "mm/alloc.h"
#include "mm/paging/paging.h"

/**
 * @brief Initialize a page directory
 *
 * @param page_dir  Pointer to page directory
 * @return int
 */
void
page_dir_init(page_dir_t page_dir)
{
    for (int i = 0; i < PAGING_PAGE_DIR_ENTRIES; i++) {
        page_dir_entry_t page_dir_entry = (i * PAGING_PAGE_SIZE * PAGING_PAGE_TBL_ENTRIES);
        page_dir_entry &= 0xFFFFF000; // Clear flags
        page_dir[i] = page_dir_entry;
    }
}

page_dir_entry_t
page_dir_get_entry(page_dir_t page_dir, void *vaddr)
{
    uint32_t dir_entry_idx = ((uint32_t)vaddr / (PAGING_PAGE_DIR_ENTRIES * PAGING_PAGE_SIZE));
    page_dir_entry_t page_dir_entry = page_dir[dir_entry_idx];
    return page_dir_entry;
}

int
page_dir_create_new_entry(page_dir_t page_dir, void *vaddr)
{
    uint32_t dir_entry_idx = ((uint32_t)vaddr / (PAGING_PAGE_DIR_ENTRIES * PAGING_PAGE_SIZE));
    page_dir_entry_t current_entry = page_dir[dir_entry_idx];

    // Entry already exists
    if (current_entry) {
        return -EEXIST;
    }

    page_dir_entry_t page_dir_entry = (page_dir_entry_t)alloc_get_phys_page();

    page_dir_entry &= 0xFFFFF000;          // Clear flags
    page_dir_entry |= PAGING_PAGE_PRESENT; // Ensure the entry is marked as present

    page_dir[dir_entry_idx] = page_dir_entry;

    return 0;
}
