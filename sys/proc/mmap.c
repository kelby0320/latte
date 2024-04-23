#include "proc/mmap.h"

#include "errno.h"
#include "libk/list.h"
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

    struct process_allocation *palloc = kzalloc(sizeof(struct process_allocation));
    palloc->type = PROCESS_ALLOCATION_MEMORY_SEGMENT;
    palloc->addr = virt;
    palloc->size = segment_size;

    list_append(&process->allocations, palloc);

    return virt;

err_out:
    return NULL;
}

int
process_munmap(struct process *process, void *addr, size_t length)
{
    // Find the corresponding allocation
    struct process_allocation *palloc = NULL;
    for_each_in_list(struct process_allocation *, process->allocations, list, allocation)
    {
        if (allocation->addr == addr) {
            palloc = allocation;
            break;
        }
    }

    if (!palloc) {
        return -ENOENT;
    }

    // Remove the allocation from the allcation list
    list_remove(&process->allocations, palloc);

    // Unmap the allocation
    vm_area_unmap_pages(process->vm_area, palloc->addr, palloc->size);

    // Free physcal memory
    void *paddr = vm_area_virt_to_phys(process->vm_area, palloc->addr);
    kalloc_free_phys_pages(paddr);

    // Free the allocation object
    kfree(palloc);

    return 0;
}