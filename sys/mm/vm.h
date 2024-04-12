#ifndef VM_H
#define VM_H

#include "mm/paging/page_tbl.h"
#include "mm/paging/paging.h"

#include <stddef.h>
#include <stdint.h>

#define VM_PAGE_WRITABLE PAGING_PAGE_WRITABLE
#define VM_PAGE_USER     PAGING_PAGE_USER

#define KERNEL_HIGHER_HALF_START 0xC0000000
#define KERNEL_HEAP_VADDR_START ((void *)0xC4400000) // 3GB + 68MB
#define USER_HEAP_VADDR_START   ((void *)0x01000000) // 16MB

#define vm_area_map_kernel_page(phys) \
    vm_area_map_page(&kernel_vm_area, KERNEL_HEAP_VADDR_START, phys, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE)

#define vm_area_map_kernel_pages(phys, size) \
    vm_area_map_pages(&kernel_vm_area, KERNEL_HEAP_VADDR_START, phys, size, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE)

#define vm_area_map_kernel_large_pages(phys, num_large_pages) \
    vm_area_map_large_pages(&kernel_vm_area, KERNEL_HEAP_VADDR_START, phys, num_large_pages, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE)

#define vm_area_map_user_page(vm_area, phys) \
    vm_area_map_page(vm_area, USER_HEAP_VADDR_START, phys, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER)

#define vm_area_map_user_pages(vm_area, phys, size) \
    vm_area_map_pages(vm_area, USER_HEAP_VADDR_START, phys, size, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER)

#define vm_area_map_user_large_pages(vm_area, phys, num_large_pages) \
    vm_area_map_large_pages(vm_area, USER_HEAP_VADDR_START, phys, num_large_pages, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER)

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

void *
vm_area_map_page(struct vm_area *vm_area, void *base_vaddr, void *phys, uint8_t flags);

void *
vm_area_map_pages(struct vm_area *vm_area, void *base_vaddr, void *phys, size_t size, uint8_t flags);

void *
vm_area_map_large_pages(struct vm_area *vm_area, void *base_vaddr, void **phys, size_t num_large_pages,
                uint8_t flags);

void
vm_area_unmap_page(struct vm_area *vm_area, void *vaddr);

void
vm_area_unmap_pages(struct vm_area *vm_area, void *vaddr, size_t size);

void
vm_area_unmap_large_pages(struct vm_area *vm_area, void **vaddrs, size_t num_large_pages);

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

#endif