#include "mem/vm.h"

#include "errno.h"
#include "libk/kheap.h"

/**
 * @brief Free page tables
 *
 * @param page_dir  Pointer to page directory
 */
static void
free_page_dir_entries(uint32_t *page_dir)
{
    for (int i = 0; i < VM_PAGE_DIR_ENTRIES; i++) {
        uint32_t dir_entry = page_dir[i];
        uint32_t *page_tbl = (uint32_t *)(dir_entry & 0xfffff000);
        if (page_tbl) {
            kfree(page_tbl);
        }
    }
}

/**
 * @brief Initialize a page table
 *
 * @param page_tbl      Pointer to page table
 * @param tbl_offset    Physical memory page table offset
 * @param flags         Page flags
 */
static void
init_page_tbl(uint32_t *page_tbl, int tbl_offset, uint8_t flags)
{
    for (int i = 0; i < VM_PAGE_TBL_ENTRIES; i++) {
        uint32_t tbl_entry = (tbl_offset + (i * VM_PAGE_SIZE)) | flags;
        page_tbl[i] = tbl_entry;
    }
}

/**
 * @brief Initialize a page directory
 *
 * @param page_dir  Pointer to page directory
 * @param flags     Page flags
 * @return int
 */
static int
init_page_dir(uint32_t *page_dir, uint8_t flags)
{
    int res = 0;
    int tbl_offset = 0;

    for (int i = 0; i < VM_PAGE_DIR_ENTRIES; i++) {
        uint32_t *page_tbl = kzalloc(sizeof(uint32_t) * VM_PAGE_TBL_ENTRIES);
        if (!page_tbl) {
            res = -ENOMEM;
            goto err_out;
        }

        init_page_tbl(page_tbl, tbl_offset, flags);

        tbl_offset += VM_PAGE_TBL_ENTRIES * VM_PAGE_SIZE;
        uint32_t dir_entry = (uint32_t)page_tbl | flags | VM_PAGE_WRITABLE;
        page_dir[i] = dir_entry;
    }

    return 0;

err_out:
    free_page_dir_entries(page_dir);
    return res;
}

/**
 * @brief Get page directory index and page table index from a virtual address
 *
 * @param virt          Virtual address
 * @param dir_idx_out   Output page directory index
 * @param tbl_idx_out   Output page table index
 * @return int          Status code
 */
static int
get_page_indices(void *virt, uint32_t *dir_idx_out, uint32_t *tbl_idx_out)
{
    if (!is_aligned(virt)) {
        return -EINVAL;
    }

    *dir_idx_out = ((uint32_t)virt / (VM_PAGE_DIR_ENTRIES * VM_PAGE_SIZE));
    *tbl_idx_out = ((uint32_t)virt % (VM_PAGE_DIR_ENTRIES * VM_PAGE_SIZE) / VM_PAGE_SIZE);

    return 0;
}

/**
 * @brief Load a new page directory
 *
 * Defined in vm.S
 *
 * @param page_dir  Pointer to page directory
 */
void
load_page_directory(uint32_t *page_dir);

int
vm_area_init(struct vm_area *vm_area, uint8_t flags)
{
    uint32_t *page_dir = kzalloc(sizeof(uint32_t) * VM_PAGE_DIR_ENTRIES);
    if (!page_dir) {
        return -ENOMEM;
    }

    int res = init_page_dir(page_dir, flags);
    if (res < 0) {
        kfree(page_dir);
        return res;
    }

    vm_area->page_directory = page_dir;

    return 0;
}

void
vm_area_free(struct vm_area *vm_area)
{
    uint32_t *page_dir = vm_area->page_directory;
    free_page_dir_entries(page_dir);
    kfree(vm_area->page_directory);
}

uint32_t
vm_area_get_page_entry(uint32_t *page_dir, void *virt)
{
    if (!is_aligned(virt)) {
        return -EINVAL;
    }

    uint32_t dir_idx;
    uint32_t tbl_idx;
    int res = get_page_indices(virt, &dir_idx, &tbl_idx);
    if (res < 0) {
        return res;
    }

    uint32_t dir_entry = page_dir[dir_idx];
    uint32_t *page_tbl = (uint32_t *)(dir_entry & 0xfffff000);
    return page_tbl[tbl_idx];
}

int
vm_area_set_page_entry(uint32_t *page_dir, void *virt, uint32_t val)
{
    if (!is_aligned(virt)) {
        return -EINVAL;
    }

    uint32_t dir_idx;
    uint32_t tbl_idx;
    int res = get_page_indices(virt, &dir_idx, &tbl_idx);
    if (res < 0) {
        return res;
    }

    uint32_t dir_entry = page_dir[dir_idx];
    uint32_t *page_tbl = (uint32_t *)(dir_entry & 0xfffff000);
    page_tbl[tbl_idx] = val;

    return 0;
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

    uint32_t val = (uint32_t)phys | flags;

    int res = vm_area_set_page_entry(vm_area->page_directory, virt, val);
    if (res < 0) {
        return -EINVAL;
    }

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

        virt += VM_PAGE_SIZE;
        phys += VM_PAGE_SIZE;
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
    size_t total_pages = total_bytes / VM_PAGE_SIZE;
    return vm_area_map_range(vm_area, virt, phys, total_pages, flags);
}
