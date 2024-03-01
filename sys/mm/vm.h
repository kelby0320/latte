#ifndef VM_H
#define VM_H

#include "mm/paging/paging.h"
#include "mm/paging/page_tbl.h"

#include <stddef.h>
#include <stdint.h>

#define VM_PAGE_WRITABLE    PAGING_PAGE_WRITABLE
#define VM_PAGE_USER        PAGING_PAGE_USER

/**
 * @brief VM Area is a wrapper around a page directory
 *
 */
struct vm_area {
    uint32_t *page_directory;
};

/**
 * @brief Initialize a new vm_area from an existing page directory
 * 
 * @param vm_area 
 * @param page_dir 
 */
void
vm_area_from_page_directory(struct vm_area *vm_area, uint32_t *page_dir);

/**
 * @brief Initialize a new vm area
 *
 * @param vm_area   Pointer to vm area
 * @return int      Status code
 */
int
vm_area_init(struct vm_area *vm_area);

/**
 * @brief Free a vm_area
 *
 * @param vm_area   Pointer to vm area
 */
void
vm_area_free(struct vm_area *vm_area);

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

#endif