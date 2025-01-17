#include "paging/page_tbl.h"

#include "paging.h"

#include <stdint.h>

page_tbl_entry_t
page_tbl_get_entry(page_tbl_t page_tbl, void *vaddr)
{
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
    uint32_t page_tbl_idx = ((uint32_t)vaddr & PAGING_PAGE_TBL_MASK) >> 12;

    page_tbl[page_tbl_idx] = page_tbl_entry;
}
