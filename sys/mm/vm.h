#ifndef VM_H
#define VM_H

#include "mm/paging/page_tbl.h"
#include "mm/paging/paging.h"

#include <stddef.h>
#include <stdint.h>

#define VM_PAGE_WRITABLE PAGING_PAGE_WRITABLE
#define VM_PAGE_USER     PAGING_PAGE_USER

/**
 * @brief VM Area is a wrapper around a page directory
 *
 */
struct vm_area {
    page_dir_t page_directory;
};

/**
 * @brief Initialize a new vm_area from an existing page directory
 *
 * @param vm_area
 * @param page_dir
 */
void
vm_area_kernel_init(struct vm_area *vm_area, page_dir_t page_dir);

/**
 * @brief Initialize a new vm area
 *
 * @param vm_area   Pointer to vm area
 * @return int      Status code
 */
int
vm_area_user_init(struct vm_area *vm_area);

/**
 * @brief Load the page directory from a vm area
 *
 * @param vm_area   Pointer to vm area
 */
void
vm_area_switch_map(struct vm_area *vm_area);

void *
vm_area_virt_to_phys(struct vm_area *vm_area, void *virt);

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
vm_area_map_page_to(struct vm_area *vm_area, void *virt, void *phys, uint8_t flags);

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
vm_area_map_page_range(struct vm_area *vm_area, void *virt, void *phys, size_t count,
                       uint8_t flags);

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
vm_area_map_pages_to(struct vm_area *vm_area, void *virt, void *phys, void *phys_end,
                     uint8_t flags);

/**
 * @brief   Map a single page of kernel memory
 *
 * @param vm_area   Pointer to vm area
 * @param phys      Physical address
 * @return void*    Virtual address of kernel page
 */
void *
vm_area_map_kernel_page(void *phys);

/**
 * @brief   Map a set of contiguous pages of kernel memory
 *
 * @param phys      Physical address
 * @param size      Size of the memory to map (must be a multiple of page size)
 * @return void*    Virtual address of the start of contiguously allocated virtual memory
 */
void *
vm_area_map_kernel_pages(void *phys, size_t size);

/**
 * @brief   Map a set of large pages of kernel memory
 *
 * Note: A large page is an entire page table, e.g. 4MB
 *
 * @param vm_area           Pointer to vm area
 * @param phys              Array of physical addresses
 * @param num_large_pages   Length of the array
 * @return void*            Virtual address of the start of contiguously allocated virtual memory
 */
void *
vm_area_map_kernel_large_pages(void **phys, size_t num_large_pages);

/**
 * @brief           Map a single page of user memory
 *
 * @param vm_area   Pointer to vm area
 * @param phys      Physical address
 * @return void*    Virtual address of user page
 */
void *
vm_area_map_user_page(struct vm_area *vm_area, void *phys);

/**
 * @brief        Map a set of contiguous pages of user memory
 *
 * @param vm_area   Pointer to vm area
 * @param phys      Physical address
 * @param size      Size of the memory to map (must be a multiple of page size)
 * @return void*    Virtual address of the start of contiguously allocated virtual memory
 */
void *
vm_area_map_user_pages(struct vm_area *vm_area, void *phys, size_t size);

/**
 * @brief                   Map a set of large pages of user memory
 *
 * Note: A large page is an entire page table, e.g. 4MB
 *
 * @param vm_area           Pointer to vm area
 * @param phys              Array of physical addresses
 * @param num_large_pages   Length of the array
 * @return void*            Virtual address of the start of contiguously allocated virtual memory
 */
void *
vm_area_map_user_large_pages(struct vm_area *vm_area, void **phys, size_t num_large_pages);

#endif