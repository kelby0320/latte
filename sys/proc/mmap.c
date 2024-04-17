#include "proc/mmap.h"

#include "mm/kalloc.h"
#include "mm/vm.h"
#include "proc/mgmt.h"

#include "libk/alloc.h"
#include "mm/paging/paging.h"
#include "proc/process.h"

void *
process_mmap(struct process *process, void *addr, size_t size, int prot, int flags, int fd,
             size_t offset)
{
    if (size % PAGING_PAGE_SIZE) {
        size = (size + PAGING_PAGE_SIZE) - (size % PAGING_PAGE_SIZE);
    }

    int order = kalloc_size_to_order(size);
    void *segment = kalloc_get_phys_pages(order);
    if (!segment) {
        goto err_out;
    }
    size_t segment_size = kalloc_order_to_size(order);

    void *virt = vm_area_map_user_pages(process->vm_area, segment, segment_size);
    if (!virt) {
        kfree(segment);
        goto err_out;
    }

    struct process_allocation palloc = {
        .type = PROCESS_ALLOCATION_MEMORY_SEGMENT, .addr = virt, .size = segment_size};
    list_item_append(process_allocation_t, &process->allocations, palloc);

    return virt;

err_out:
    return NULL;
}

int
process_munmap(struct process *process, void *addr, size_t length)
{
    // TODO

    return -1;
}