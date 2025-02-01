#include "paging/page_dir.h"

#include "errno.h"
#include "kalloc.h"
#include "libk/memory.h"
#include "paging.h"
#include "paging/page_tbl.h"
#include "vm.h"

page_dir_entry_t
page_dir_get_entry(page_dir_t page_dir, void *vaddr)
{
    uint32_t dir_entry_idx = ((uint32_t)vaddr & PAGING_PAGE_DIR_MASK) >> 22;
    page_dir_entry_t page_dir_entry = page_dir[dir_entry_idx];
    return page_dir_entry;
}

void
page_dir_set_entry(
    page_dir_t page_dir, void *vaddr, page_dir_entry_t page_dir_entry)
{
    uint32_t dir_entry_idx = ((uint32_t)vaddr & PAGING_PAGE_DIR_MASK) >> 22;

    page_dir[dir_entry_idx] = page_dir_entry;
}

page_dir_entry_t
page_dir_add_page_tbl(
    page_dir_t page_dir, void *vaddr, page_tbl_t page_tbl, uint8_t flags)
{
    uint32_t dir_entry_idx = ((uint32_t)vaddr & PAGING_PAGE_DIR_MASK) >> 22;
    page_dir_entry_t current_entry = page_dir[dir_entry_idx];

    // Entry already exists
    if (current_entry) {
        return -EEXIST;
    }

    page_dir_entry_t page_dir_entry = (page_dir_entry_t)page_tbl;
    page_dir_entry |= flags;

    page_dir[dir_entry_idx] = page_dir_entry;

    return page_dir_entry;
}
