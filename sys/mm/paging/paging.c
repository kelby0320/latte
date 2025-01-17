#include "paging.h"

#include "kernel.h"
#include "libk/memory.h"
#include "kalloc.h"
#include "paging/page_dir.h"
#include "paging/page_tbl.h"

#define is_page_dir_entry_free(page_dir_entry) (((uint32_t)page_dir_entry & 0xFFFFF000) == 0)
#define is_page_tbl_entry_free(page_tbl_entry) (((uint32_t)page_tbl_entry & 0xFFFFF000) == 0)

static void *
paging_find_free_large_extent(page_dir_t page_dir, bool is_kernel_addr, size_t num_large_pages)
{
    return NULL;
}

static void *
find_free_extent_in_page_tbl(page_tbl_t page_tbl, void *starting_addr, size_t num_pages)
{
    void *search_vaddr = starting_addr;
    size_t extent_pages = 0;

    for (size_t i = 0; i < PAGING_PAGE_TBL_ENTRIES && extent_pages < num_pages; i++) {
        page_tbl_entry_t page_tbl_entry = page_tbl_get_entry(page_tbl, search_vaddr);
        if (is_page_tbl_entry_free(page_tbl_entry)) {
            extent_pages++;
        } else {
            // Reset search
            search_vaddr += PAGING_PAGE_SIZE * (extent_pages + 1);
            extent_pages = 0;
        }
    }

    if (extent_pages != num_pages) {
        search_vaddr = NULL;
    }

    return search_vaddr;
}

static void *
find_free_extent_in_page_dir(page_dir_t page_dir, void *starting_addr, size_t num_pages)
{

    void *search_vaddr = starting_addr;
    size_t total_num_pages = num_pages;
    size_t num_page_dirs = (num_pages / PAGING_PAGE_TBL_ENTRIES) + 1;
    size_t extent_dirs = 0;

    for (size_t i = 0; i < PAGING_PAGE_DIR_ENTRIES && extent_dirs < num_page_dirs; i++) {
        page_dir_entry_t page_dir_entry = page_dir_get_entry(page_dir, search_vaddr);

        if (is_page_dir_entry_free(page_dir_entry)) {
            // Add a new page table to the directory
            page_tbl_t page_tbl = kalloc_get_phys_page();
            page_dir_entry = page_dir_add_page_tbl(page_dir, search_vaddr, page_tbl,
                                                   PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE);
        }

        page_tbl_t page_tbl = page_tbl_from_page_dir_entry(page_dir_entry);
        size_t num_pages_to_find =
            total_num_pages < PAGING_PAGE_TBL_ENTRIES ? total_num_pages : PAGING_PAGE_TBL_ENTRIES;

        void *extent_addr = find_free_extent_in_page_tbl(page_tbl, search_vaddr, num_pages_to_find);
        if (!extent_addr) {
            // Reset search
            search_vaddr += PAGING_PAGE_TBL_ENTRIES * PAGING_PAGE_SIZE * (extent_dirs + 1);
            extent_dirs = 0;
            total_num_pages = num_pages;
        } else {
            // Continue search
            if (extent_addr != search_vaddr) {
                search_vaddr = extent_addr;
            }

            extent_dirs++;
            total_num_pages -= num_pages_to_find;
        }
    }

    if (extent_dirs != num_page_dirs) {
        search_vaddr = NULL;
    }

    return search_vaddr;
}

void
paging_copy_kernel_pages_to_user(page_dir_t kernel_page_dir, page_dir_t user_page_dir)
{
    for (int i = 0; i < PAGING_KERNEL_DIR_ENTRIES; i++) {
        page_dir_entry_t kernel_page_dir_entry = kernel_page_dir[i + PAGING_KERNEL_DIR_OFFSET];
        user_page_dir[i + PAGING_KERNEL_DIR_OFFSET] = kernel_page_dir_entry;
    }
}

void *
paging_find_free_page(page_dir_t page_dir, void *base_vaddr)
{
    void *extent_addr = find_free_extent_in_page_dir(page_dir, base_vaddr, 1);
    return extent_addr;
}

void *
paging_find_free_extent(page_dir_t page_dir, void *base_vaddr, size_t num_pages)
{
    if (num_pages >= PAGING_PAGE_TBL_ENTRIES) {
        size_t num_large_pages = (num_pages / PAGING_PAGE_TBL_ENTRIES) + 1;
        return paging_find_free_large_extent(page_dir, base_vaddr, num_large_pages);
    }

    void *extent_addr = find_free_extent_in_page_dir(page_dir, base_vaddr, num_pages);
    return extent_addr;
}
