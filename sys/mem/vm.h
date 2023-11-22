#ifndef VM_H
#define VM_H

#include <stddef.h>
#include <stdint.h>

#define VM_PAGE_PRESENT         0b00000001
#define VM_PAGE_WRITABLE        0b00000010
#define VM_PAGE_SUPERVISOR      0b00000100
#define VM_PAGE_WRITE_THROUGH   0b00001000
#define VM_PAGE_CACHE_DISABLED  0b00010000
#define VM_PAGE_ACCESSED        0b00100000
#define VM_PAGE_DIRTY           0b01000000

#define VM_PAGE_SIZE            4096
#define VM_PAGE_TBL_ENTRIES     1024
#define VM_PAGE_DIR_ENTRIES     1024

#define is_aligned(addr) (((uint32_t)addr % VM_PAGE_SIZE) == 0)


struct vm_area {
    uint32_t *page_directory;
};

int
vm_area_init(struct vm_area* vm_area, uint8_t flags);

void
vm_area_free(struct vm_area* vm_area);

void
vm_area_switch_map(struct vm_area *vm_area);

int
vm_area_map_page(struct vm_area *vm_area, void *virt, void *phys, uint8_t flags);

int
vm_area_map_range(struct vm_area *vm_area, void *virt, void *phys, size_t count, uint8_t flags);

int
vm_area_map_to(struct vm_area *vm_area, void *virt, void *phys, void *phys_end, uint8_t flags);

void
enable_paging();

#endif