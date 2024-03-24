#include "mm/vm.h"

#include "errno.h"
#include "kernel.h"
#include "mm/kalloc.h"
#include "mm/paging/page_dir.h"
#include "mm/paging/page_tbl.h"
#include "mm/paging/paging.h"

#include <stdint.h>

static void *
map_page(struct vm_area *vm_area, void *base_vaddr, void *phys, uint8_t flags)
{
    void *free_page = paging_find_free_page(vm_area->page_directory, base_vaddr);
    if (!free_page) {
        return NULL;
    }

    int res = vm_area_map_page_to(vm_area, free_page, phys, flags);
    if (res < 0) {
        return NULL;
    }

    return free_page;
}

static void *
map_large_pages(struct vm_area *vm_area, void *base_vaddr, void **phys, size_t num_large_pages,
                uint8_t flags)
{
    void *free_extent = paging_find_free_extent(vm_area->page_directory, base_vaddr,
                                                num_large_pages * PAGING_LARGE_PAGE_SIZE);
    if (!free_extent) {
        return NULL;
    }

    for (size_t i = 0; i < num_large_pages; i++) {
        int res = vm_area_map_pages_to(vm_area, free_extent, phys[i],
                                       phys[i] + PAGING_LARGE_PAGE_SIZE, flags);
        if (res < 0) {
            // This needs to be cleaned up if it fails
            return NULL;
        }
    }

    return free_extent;
}

void
vm_area_from_page_directory(struct vm_area *vm_area, uint32_t *page_dir)
{
    vm_area->page_directory = page_dir;
}

int
vm_area_init(struct vm_area *vm_area)
{
    uint32_t *page_dir = kalloc_get_phys_page();
    if (!page_dir) {
        return -ENOMEM;
    }

    page_dir_init(page_dir);

    vm_area->page_directory = page_dir;

    return 0;
}

void
vm_area_free(struct vm_area *vm_area)
{
    // TODO: Free all page tables
}

void *
vm_area_virt_to_phys(struct vm_area *vm_area, void *virt)
{
}

void
vm_area_switch_map(struct vm_area *vm_area)
{
    paging_load_page_directory(vm_area->page_directory);
}

int
vm_area_map_page_to(struct vm_area *vm_area, void *virt, void *phys, uint8_t flags)
{
    if (!is_aligned(virt) || !is_aligned(phys)) {
        return -EINVAL;
    }

    page_dir_entry_t page_dir_entry = page_dir_get_entry(vm_area->page_directory, virt);
    if (page_dir_entry == 0) {
        // Page directory entry does not exist, create it
        page_dir_entry = page_dir_create_new_entry(vm_area->page_directory, virt);
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

void *
vm_area_map_kernel_page(void *phys)
{
    return map_page(&kernel_vm_area, KERNEL_HEAP_VADDR_START, phys,
                    PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE);
}

void *
vm_area_map_kernel_large_pages(void **phys, size_t num_large_pages)
{
    return map_large_pages(&kernel_vm_area, KERNEL_HEAP_VADDR_START, phys, num_large_pages,
                           PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE);
}

void *
vm_area_map_user_page(struct vm_area *vm_area, void *phys)
{
    return map_page(vm_area, USER_HEAP_VADDR_START, phys,
                    PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER);
}

void *
vm_area_map_user_large_pages(struct vm_area *vm_area, void **phys, size_t num_large_pages)
{
    return map_large_pages(vm_area, USER_HEAP_VADDR_START, phys, num_large_pages,
                           PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER);
}
