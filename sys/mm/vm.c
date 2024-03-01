#include "mm/vm.h"

#include "errno.h"
#include "mm/alloc.h"
#include "mm/paging/page_dir.h"
#include "mm/paging/page_tbl.h"

#include <stdint.h>

void
vm_area_from_page_directory(struct vm_area *vm_area, uint32_t *page_dir)
{
    vm_area->page_directory = page_dir;
}

int
vm_area_init(struct vm_area *vm_area)
{
    uint32_t *page_dir = alloc_get_phys_page();
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

void
vm_area_switch_map(struct vm_area *vm_area)
{
    load_page_directory(vm_area->page_directory);
}

int
vm_area_map_page(struct vm_area *vm_area, void *virt, void *phys, uint8_t flags)
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
vm_area_map_range(struct vm_area *vm_area, void *virt, void *phys, size_t num_pages, uint8_t flags)
{
    if (!is_aligned(virt) || !is_aligned(phys)) {
        return -EINVAL;
    }

    for (size_t i = 0; i < num_pages; i++) {
        int res = vm_area_map_page(vm_area, virt, phys, flags);
        if (res < 0) {
            return res;
        }

        virt += PAGING_PAGE_SIZE;
        phys += PAGING_PAGE_SIZE;
    }

    return 0;
}

int
vm_area_map_to(struct vm_area *vm_area, void *virt, void *phys, void *phys_end, uint8_t flags)
{
    if (!is_aligned(virt) || !is_aligned(phys) || !is_aligned(phys_end)) {
        return -EINVAL;
    }

    if ((uint32_t)phys_end < (uint32_t)phys) {
        return -EINVAL;
    }

    uint32_t total_bytes = phys_end - phys;
    size_t total_pages = total_bytes / PAGING_PAGE_SIZE;
    return vm_area_map_range(vm_area, virt, phys, total_pages, flags);
}
