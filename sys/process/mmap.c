#include "mmap.h"

#include "errno.h"
#include "kalloc.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/memory.h"
#include "paging.h"
#include "process.h"
#include "vm.h"

void *
process_mmap(
    struct process *process, void *addr, size_t size, int prot, int flags,
    int fd, size_t offset)
{
    int order = kalloc_size_to_order(size);
    void *segment = kalloc_get_phys_pages(order);
    if (!segment) {
        goto err_out;
    }
    size_t segment_size = kalloc_order_to_size(order);

    memset(segment, 0, segment_size);

    void *virt =
        vm_area_map_user_pages(process->vm_area, segment, segment_size);
    if (!virt) {
        kalloc_free_phys_pages(segment);
        goto err_out;
    }

    struct process_allocation *palloc =
        kzalloc(sizeof(struct process_allocation));
    palloc->type = PROCESS_ALLOCATION_MEMORY_SEGMENT;
    palloc->paddr = segment;
    palloc->vaddr = virt;
    palloc->size = segment_size;

    list_push_front(&process->allocations, palloc);

    return virt;

err_out:
    return NULL;
}

int
process_munmap(struct process *process, void *addr, size_t length)
{
    // Find the corresponding allocation
    struct process_allocation *palloc = NULL;
    for_each_in_list(
        struct process_allocation *, process->allocations, list, allocation)
    {
        if (allocation->vaddr == addr) {
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
    vm_area_unmap_pages(process->vm_area, palloc->vaddr, palloc->size);

    // Free physcal memory
    void *paddr = vm_area_virt_to_phys(process->vm_area, palloc->vaddr);
    kalloc_free_phys_pages(paddr);

    // Free the allocation object
    kfree(palloc);

    return 0;
}
