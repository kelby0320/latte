#ifndef VM_H
#define VM_H

#include "mm/paging/page_tbl.h"
#include "mm/paging/paging.h"

#include <stddef.h>
#include <stdint.h>

#define VM_PAGE_WRITABLE PAGING_PAGE_WRITABLE
#define VM_PAGE_USER     PAGING_PAGE_USER

#define KERNEL_HIGHER_HALF_START 0xC0000000
#define KERNEL_HEAP_VADDR_START  ((void *)0xC4400000) // 3GB + 68MB
#define USER_HEAP_VADDR_START    ((void *)0x01000000) // 16MB

#define vm_area_map_kernel_page(phys)                                                              \
    vm_area_map_page(&kernel_vm_area, KERNEL_HEAP_VADDR_START, phys,                               \
                     PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE)

#define vm_area_map_kernel_pages(phys, size)                                                       \
    vm_area_map_pages(&kernel_vm_area, KERNEL_HEAP_VADDR_START, phys, size,                        \
                      PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE)

#define vm_area_map_kernel_large_pages(phys, num_large_pages)                                      \
    vm_area_map_large_pages(&kernel_vm_area, KERNEL_HEAP_VADDR_START, phys, num_large_pages,       \
                            PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE)

#define vm_area_map_user_page(vm_area, phys)                                                       \
    vm_area_map_page(vm_area, USER_HEAP_VADDR_START, phys,                                         \
                     PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER)

#define vm_area_map_user_pages(vm_area, phys, size)                                                \
    vm_area_map_pages(vm_area, USER_HEAP_VADDR_START, phys, size,                                  \
                      PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER)

#define vm_area_map_user_large_pages(vm_area, phys, num_large_pages)                               \
    vm_area_map_large_pages(vm_area, USER_HEAP_VADDR_START, phys, num_large_pages,                 \
                            PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER)

/**
 * @brief VM Area is a wrapper around a page directory
 *
 */
struct vm_area {
    page_dir_t page_directory;
};

/**
 * @brief Initialize the kernel's vm_area
 *
 * @param vm_area   Pointer to vm area
 * @param page_dir  Page directory
 */
void
vm_area_kernel_init(struct vm_area *vm_area, page_dir_t page_dir);

/**
 * @brief Initialize a new vm area for a user process
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

/**
 * @brief   Convert a virtual address to a physical address
 *
 * @param vm_area   Pointer to vm area
 * @param vaddr     Virtual address
 * @return void*    Physical address
 */
void *
vm_area_virt_to_phys(struct vm_area *vm_area, void *vaddr);

/**
 * @brief   Map a physical page to a virtual address
 *
 * @param vm_area       Pointer to vm area
 * @param base_vaddr    Base virtual address to search for a free virtual address
 * @param phys          Physical address of the page
 * @param flags         Flags to set for the page
 * @return void*        Virtual address of the mapped page
 */
void *
vm_area_map_page(struct vm_area *vm_area, void *base_vaddr, void *phys, uint8_t flags);

/**
 * @brief   Map a range of physical pages to a virtual address
 *
 * @param vm_area       Pointer to vm area
 * @param base_vaddr    Base virtual address to search for free virtual addresses
 * @param phys          Physical address of the first page
 * @param size          Size of the range to map
 * @param flags         Flags to set for the pages
 * @return void*        Virtual address of the mapped pages
 */
void *
vm_area_map_pages(struct vm_area *vm_area, void *base_vaddr, void *phys, size_t size,
                  uint8_t flags);

/**
 * @brief   Map a range of large pages (4MB) to a virtual address
 *
 * @param vm_area           Pointer to vm area
 * @param base_vaddr        Base virtual address to search for free virtual addresses
 * @param phys              Array of physical addresses of the large pages
 * @param num_large_pages   Number of large pages to map
 * @param flags             Flags to set for the pages
 * @return void*            Virtual address of the mapped large pages
 */
void *
vm_area_map_large_pages(struct vm_area *vm_area, void *base_vaddr, void **phys,
                        size_t num_large_pages, uint8_t flags);

/**
 * @brief   Unmap a page
 *
 * @param vm_area   Pointer to vm area
 * @param vaddr     Virtual address
 */
void
vm_area_unmap_page(struct vm_area *vm_area, void *vaddr);

/**
 * @brief   Unmap a range of pages
 *
 * @param vm_area   Pointer to vm area
 * @param vaddr     Virtual address
 * @param size      Size of the range to unmap
 */
void
vm_area_unmap_pages(struct vm_area *vm_area, void *vaddr, size_t size);

/**
 * @brief   Unmap a range of large pages
 *
 * @param vm_area           Pointer to vm area
 * @param vaddrs            Array of virtual addresses
 * @param num_large_pages   Number of large pages to unmap
 */
void
vm_area_unmap_large_pages(struct vm_area *vm_area, void **vaddrs, size_t num_large_pages);

/**
 * @brief   Map a physical page to a virtual address
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
 * @brief   Map a range of pages to a virtual address
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
 * @brief   Map a set of physical pages to a virtual address
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