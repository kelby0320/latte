#include "mm/vm.h"

#include "errno.h"
#include "kernel/kernel.h"
#include "libk/alloc.h"
#include "libk/memory.h"
#include "mm/kalloc.h"
#include "mm/paging/page_dir.h"
#include "mm/paging/page_tbl.h"
#include "mm/paging/paging.h"

#include <stdint.h>

extern struct vm_area kernel_vm_area;

void
vm_area_kernel_init(struct vm_area *vm_area, uint32_t *page_dir)
{
    page_dir_t phys_page_dir = (page_dir_t)((uint32_t)page_dir - KERNEL_HIGHER_HALF_START);
    vm_area->page_directory = phys_page_dir;
}

int
vm_area_user_init(struct vm_area *vm_area)
{
    page_dir_t page_dir = (page_dir_t)kalloc_get_phys_page();
    if (!page_dir) {
        return -ENOMEM;
    }

    memset(page_dir, 0, PAGING_PAGE_SIZE);

    vm_area->page_directory = page_dir;

    return 0;
}

int
vm_area_free(struct vm_area *vm_area)
{
    // TODO
    return 0;
}

void
vm_area_switch_map(struct vm_area *vm_area)
{
    paging_load_page_directory(vm_area->page_directory);
}

void *
vm_area_virt_to_phys(struct vm_area *vm_area, void *vaddr)
{
    page_dir_entry_t page_dir_entry = page_dir_get_entry(vm_area->page_directory, vaddr);
    if (page_dir_entry == 0) {
        return NULL;
    }

    page_tbl_t page_tbl = page_tbl_from_page_dir_entry(page_dir_entry);
    page_tbl_entry_t page_tbl_entry = page_tbl_get_entry(page_tbl, vaddr);
    if (page_tbl_entry == 0) {
        return NULL;
    }

    return (void *)(page_tbl_entry & PAGING_PAGE_OFFSET_MASK);
}

void *
vm_area_map_page(struct vm_area *vm_area, void *base_vaddr, void *phys, uint8_t flags)
{
    void *free_page = paging_find_free_page(vm_area->page_directory, base_vaddr);
    if (!free_page) {
        return NULL;
    }

    int res = vm_area_map_page_to(vm_area, free_page, phys, flags);
    if (res < 0) {
        // This needs to be cleaned up if it fails
        return NULL;
    }

    return free_page;
}

void *
vm_area_map_pages(struct vm_area *vm_area, void *base_vaddr, void *phys, size_t size, uint8_t flags)
{
    // Size must be a multiple of the page size
    if (size % PAGING_PAGE_SIZE != 0) {
        return NULL;
    }

    size_t num_pages = size / PAGING_PAGE_SIZE;

    void *free_extent = paging_find_free_extent(vm_area->page_directory, base_vaddr, num_pages);
    if (!free_extent) {
        return NULL;
    }

    int res = vm_area_map_pages_to(vm_area, free_extent, phys, phys + size, flags);
    if (res < 0) {
        // This needs to be cleaned up if it fails
        return NULL;
    }

    return free_extent;
}

void *
vm_area_map_large_pages(struct vm_area *vm_area, void *base_vaddr, void **phys,
                        size_t num_large_pages, uint8_t flags)
{
    size_t num_pages = num_large_pages * PAGING_PAGE_TBL_ENTRIES;
    void *free_extent = paging_find_free_extent(vm_area->page_directory, base_vaddr, num_pages);
    if (!free_extent) {
        return NULL;
    }

    for (size_t i = 0; i < num_large_pages; i++) {
        void *vaddr = free_extent + (i * PAGING_LARGE_PAGE_SIZE);
        void *paddr = phys[i];
        void *paddr_end = phys + PAGING_LARGE_PAGE_SIZE;

        int res = vm_area_map_pages_to(vm_area, vaddr, paddr, paddr_end, flags);
        if (res < 0) {
            // This needs to be cleaned up if it fails
            return NULL;
        }
    }

    return free_extent;
}

void
vm_area_unmap_page(struct vm_area *vm_area, void *vaddr)
{
    page_dir_entry_t page_dir_entry = page_dir_get_entry(vm_area->page_directory, vaddr);
    if (page_dir_entry == 0) {
        return;
    }

    page_tbl_t page_tbl = page_tbl_from_page_dir_entry(page_dir_entry);
    page_tbl_entry_t page_tbl_entry = page_tbl_get_entry(page_tbl, vaddr);
    if (page_tbl_entry == 0) {
        return;
    }

    page_tbl_set_entry(page_tbl, vaddr, 0);
}

void
vm_area_unmap_pages(struct vm_area *vm_area, void *vaddr, size_t size)
{
    // Size must be a multiple of the page size
    if (size % PAGING_PAGE_SIZE != 0) {
        return;
    }

    size_t num_pages = size / PAGING_PAGE_SIZE;

    for (size_t i = 0; i < num_pages; i++) {
        void *page_vaddr = vaddr + (i * PAGING_PAGE_SIZE);
        vm_area_unmap_page(vm_area, page_vaddr);
    }
}

void
vm_area_unmap_large_pages(struct vm_area *vm_area, void **vaddrs, size_t num_large_pages)
{
    for (size_t i = 0; i < num_large_pages; i++) {
        vm_area_unmap_pages(vm_area, vaddrs[i], PAGING_LARGE_PAGE_SIZE);
    }
}

int
vm_area_map_page_to(struct vm_area *vm_area, void *virt, void *phys, uint8_t flags)
{
    if (!is_aligned(virt) || !is_aligned(phys)) {
        return -EINVAL;
    }

    page_dir_entry_t page_dir_entry = page_dir_get_entry(vm_area->page_directory, virt);

    // Create new page directory entry if on doesn't exists
    if (page_dir_entry == 0) {
        page_tbl_t page_tbl = kalloc_get_phys_page();
	memset(page_tbl, 0, PAGING_PAGE_SIZE);
        page_dir_entry = page_dir_add_page_tbl(vm_area->page_directory, virt, page_tbl, flags);
    }

    // Update flags if they are different
    if ((page_dir_entry & 0x0000000F) != flags) {
        page_dir_entry |= flags;
        page_dir_set_entry(vm_area->page_directory, virt, page_dir_entry);
    }

    page_tbl_t page_tbl = page_tbl_from_page_dir_entry(page_dir_entry);
    page_tbl_entry_t page_tbl_entry = page_tbl_get_entry(page_tbl, virt);
    if (page_tbl_entry != 0) {
        // Page already mapped
        return -EINVAL;
    }

    page_tbl_entry = (uint32_t)phys | flags;
    page_tbl_set_entry(page_tbl, virt, page_tbl_entry);

    return 0;
}

int
vm_area_map_page_range(struct vm_area *vm_area, void *virt, void *phys, size_t num_pages,
                       uint8_t flags)
{
    if (!is_aligned(virt) || !is_aligned(phys)) {
        return -EINVAL;
    }

    for (size_t i = 0; i < num_pages; i++) {
        int res = vm_area_map_page_to(vm_area, virt, phys, flags);
        if (res < 0) {
            return res;
        }

        virt += PAGING_PAGE_SIZE;
        phys += PAGING_PAGE_SIZE;
    }

    return 0;
}

int
vm_area_map_pages_to(struct vm_area *vm_area, void *virt, void *phys, void *phys_end, uint8_t flags)
{
    if (!is_aligned(virt) || !is_aligned(phys) || !is_aligned(phys_end)) {
        return -EINVAL;
    }

    if ((uint32_t)phys_end < (uint32_t)phys) {
        return -EINVAL;
    }

    uint32_t total_bytes = phys_end - phys;
    size_t total_pages = total_bytes / PAGING_PAGE_SIZE;
    return vm_area_map_page_range(vm_area, virt, phys, total_pages, flags);
}
