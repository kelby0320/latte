#include "mm/paging/page_dir.h"

#include "errno.h"
#include "mm/kalloc.h"
#include "mm/paging/page_tbl.h"
#include "mm/paging/paging.h"
#include "libk/memory.h"
#include "mm/vm.h"

page_dir_entry_t
page_dir_get_entry(page_dir_t page_dir, void *vaddr)
{
    // uint32_t dir_entry_idx = ((uint32_t)vaddr / (PAGING_PAGE_DIR_ENTRIES * PAGING_PAGE_SIZE));
    uint32_t dir_entry_idx = ((uint32_t)vaddr & PAGING_PAGE_DIR_MASK) >> 22;
    page_dir_entry_t page_dir_entry = page_dir[dir_entry_idx];
    return page_dir_entry;
}

page_dir_entry_t
page_dir_add_page_tbl(page_dir_t page_dir, void *vaddr, page_tbl_t page_tbl, uint8_t flags)
{
    // uint32_t dir_entry_idx = ((uint32_t)vaddr / (PAGING_PAGE_DIR_ENTRIES * PAGING_PAGE_SIZE));
    uint32_t dir_entry_idx = ((uint32_t)vaddr & PAGING_PAGE_DIR_MASK) >> 22;
    page_dir_entry_t current_entry = page_dir[dir_entry_idx];

    // Entry already exists
    if (current_entry) {
        return -EEXIST;
    }

    page_dir_entry_t page_dir_entry = (page_dir_entry_t)page_tbl;
    page_dir_entry &= 0xFFFFF000;                    // Clear flags
    page_dir_entry |= (flags | PAGING_PAGE_PRESENT); // Ensure the entry is marked as present

    page_dir[dir_entry_idx] = page_dir_entry;

    return page_dir_entry;
}