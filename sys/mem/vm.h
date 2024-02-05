#ifndef VM_H
#define VM_H

#include <stddef.h>
#include <stdint.h>

#define VM_PAGE_PRESENT        0b00000001
#define VM_PAGE_WRITABLE       0b00000010
#define VM_PAGE_USER           0b00000100
#define VM_PAGE_WRITE_THROUGH  0b00001000
#define VM_PAGE_CACHE_DISABLED 0b00010000
#define VM_PAGE_ACCESSED       0b00100000
#define VM_PAGE_DIRTY          0b01000000

#define VM_PAGE_SIZE        4096
#define VM_PAGE_TBL_ENTRIES 1024
#define VM_PAGE_DIR_ENTRIES 1024

#define is_aligned(addr) (((uint32_t)addr % VM_PAGE_SIZE) == 0)

/**
 * @brief VM Area is a wrapper around a page directory
 *
 */
struct vm_area {
    uint32_t *page_directory;
};

/**
 * @brief Initialize a new vm area
 *
 * @param vm_area   Pointer to vm area
 * @param flags     Flags for vm area
 * @return int      Status code
 */
int
vm_area_init(struct vm_area *vm_area, uint8_t flags);

/**
 * @brief Free a vm_area
 *
 * @param vm_area   Pointer to vm area
 */
void
vm_area_free(struct vm_area *vm_area);

/**
 * @brief Get a page table entry
 *
 * @param page_dir  Pointer to the page table
 * @param virt      Virtual address
 * @return uint32_t Page table entry
 */
uint32_t
vm_area_get_page_entry(uint32_t *page_dir, void *virt);

/**
 * @brief Set a page table entry
 *
 * @param page_dir  Pointer to the page table
 * @param virt      Virtual address
 * @param val       Value to set
 * @return int      Status code
 */
int
vm_area_set_page_entry(uint32_t *page_dir, void *virt, uint32_t val);

/**
 * @brief Load the page directory from a vm area
 *
 * @param vm_area   Pointer to vm area
 */
void
vm_area_switch_map(struct vm_area *vm_area);

/**
 * @brief Map a page
 *
 * @param vm_area   Pointer to vm area
 * @param virt      Virtual address
 * @param phys      Physical address
 * @param flags     Page flags
 * @return int      Status code
 */
int
vm_area_map_page(struct vm_area *vm_area, void *virt, void *phys, uint8_t flags);

/**
 * @brief Map a set of pages
 *
 * @param vm_area   Pointer to vm area
 * @param virt      Virtual address
 * @param phys      Physical address
 * @param count     Number of pages
 * @param flags     Page flags
 * @return int      Status code
 */
int
vm_area_map_range(struct vm_area *vm_area, void *virt, void *phys, size_t count, uint8_t flags);

/**
 * @brief Map a memory range
 *
 * @param vm_area   Pointer to vm area
 * @param virt      Virtual address
 * @param phys      Start physical address
 * @param phys_end  End physical address
 * @param flags     Page flags
 * @return int      Status code
 */
int
vm_area_map_to(struct vm_area *vm_area, void *virt, void *phys, void *phys_end, uint8_t flags);

/**
 * @brief Enable paging
 *
 * Defined in vm.S
 *
 */
void
enable_paging();

#endif