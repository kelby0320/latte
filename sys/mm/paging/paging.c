#include "mm/paging/paging.h"

#include "kernel.h"
#include "libk/memory.h"
#include "mm/paging/page_dir.h"
#include "mm/paging/page_tbl.h"

#define is_page_dir_entry_free(page_dir_entry) (((uint32_t)page_dir_entry & 0xFFFFF000) == 0)
#define is_page_tbl_entry_free(page_tbl_entry) (((uint32_t)page_tbl_entry & 0xFFFFF000) == 0)
#define is_page_free(page_vaddr)               (((uint32_t)page_vaddr & 0xFFFFF000) == 0)

static void *
find_free_page_in_tbl(page_tbl_t page_tbl, bool is_kernel_addr)
{
    void *search_vaddr = is_kernel_addr ? KERNEL_HEAP_VADDR_START : USER_HEAP_VADDR_START;
    void *top_of_tbl_vaddr = search_vaddr + (PAGING_PAGE_TBL_ENTRIES * sizeof(page_tbl_entry_t));
    page_tbl_entry_t page_tbl_entry = page_tbl_get_entry(page_tbl, search_vaddr);

    while (search_vaddr < top_of_tbl_vaddr) {
        if (is_page_tbl_entry_free(page_tbl_entry)) {
            return search_vaddr;
        }

        search_vaddr += PAGING_PAGE_SIZE;
        page_tbl_entry = page_tbl_get_entry(page_tbl, search_vaddr);
    }

    return NULL;
}

void *
paging_find_free_page(page_dir_t page_dir, bool is_kernel_addr)
{
    void *search_vaddr = is_kernel_addr ? KERNEL_HEAP_VADDR_START : USER_HEAP_VADDR_START;
    void *top_of_dir_vaddr = search_vaddr + (PAGING_PAGE_DIR_ENTRIES * sizeof(page_dir_entry_t));
    page_dir_entry_t page_dir_entry = page_dir_get_entry(page_dir, search_vaddr);

    while (search_vaddr < top_of_dir_vaddr) {
        if (is_page_dir_entry_free(page_dir_entry)) {
            page_dir_create_new_entry(page_dir, search_vaddr);
        }

        page_tbl_t page_tbl = page_tbl_from_page_dir_entry(page_dir_entry);
        void *page_vaddr = find_free_page_in_tbl(page_tbl, is_kernel_addr);
        if (page_vaddr) {
            return page_vaddr;
        }

        search_vaddr += (PAGING_PAGE_TBL_ENTRIES * PAGING_PAGE_SIZE);
        page_dir_entry = page_dir_get_entry(page_dir, search_vaddr);
    }

    return NULL;
}

void *
paging_find_free_extent(page_dir_t page_dir, bool is_kernel_addr, size_t size)
{
    // Max number of large pages is the number of page directory entries
    // i.e. 1024
    if (size > PAGING_PAGE_DIR_ENTRIES) {
        return NULL;
    }

    void *search_vaddr = is_kernel_addr ? KERNEL_HEAP_VADDR_START : USER_HEAP_VADDR_START;
    void *top_of_dir_vaddr = search_vaddr + (PAGING_PAGE_DIR_ENTRIES * sizeof(page_dir_entry_t));
    page_dir_entry_t page_dir_entry = page_dir_get_entry(page_dir, search_vaddr);

    // Stack allocated array, max size is 1024
    void *large_pages[size];
    memset(large_pages, 0, sizeof(large_pages));

    for (size_t i = 0; i < size && search_vaddr < top_of_dir_vaddr; i++) {
        if (is_page_dir_entry_free(page_dir_entry)) {
            large_pages[i] = search_vaddr;
        } else {
            // reset search
            memset(large_pages, 0, sizeof(large_pages));
            i = 0;
        }

        search_vaddr += (PAGING_PAGE_TBL_ENTRIES * PAGING_PAGE_SIZE);
        page_dir_entry = page_dir_get_entry(page_dir, search_vaddr);
    }

    // Create dir entries for each large page
    if (large_pages[size - 1]) {
        for (size_t i = 0; i < size; i++) {
            page_dir_create_new_entry(page_dir, large_pages[i]);
        }

        // return the starting address of the large pages
        return large_pages[0];
    }

    return NULL;
}