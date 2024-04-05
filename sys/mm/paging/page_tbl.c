#include "mm/paging/page_tbl.h"

#include "mm/paging/paging.h"
#include <stdint.h>

void
page_tbl_init(page_tbl_t page_tbl, uint32_t page_tbl_offset)
{
    for (int i = 0; i < PAGING_PAGE_TBL_ENTRIES; i++) {
        uint32_t tbl_entry = (page_tbl_offset + (i * PAGING_PAGE_SIZE));
        tbl_entry &= 0xFFFFF000; // Clear flags
        page_tbl[i] = tbl_entry;
    }
}

page_tbl_entry_t
page_tbl_get_entry(page_tbl_t page_tbl, void *vaddr)
{
    // uint32_t page_tbl_idx =
    //     ((uint32_t)vaddr % (PAGING_PAGE_DIR_ENTRIES * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
    uint32_t page_tbl_idx = ((uint32_t)vaddr & PAGING_PAGE_TBL_MASK) >> 12;

    if (page_tbl_idx >= PAGING_PAGE_TBL_ENTRIES) {
        return 0;
    }

    page_tbl_entry_t page_tbl_entry = page_tbl[page_tbl_idx];
    return page_tbl_entry;
}

void
page_tbl_set_entry(page_tbl_t page_tbl, void *vaddr, page_tbl_entry_t page_tbl_entry)
{
    // uint32_t page_tbl_idx =
    //     ((uint32_t)vaddr % (PAGING_PAGE_DIR_ENTRIES * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
    uint32_t page_tbl_idx = ((uint32_t)vaddr & PAGING_PAGE_TBL_MASK) >> 12;

    page_tbl_entry |= PAGING_PAGE_PRESENT; // Ensure the entry is marked as present
    page_tbl[page_tbl_idx] = page_tbl_entry;
}