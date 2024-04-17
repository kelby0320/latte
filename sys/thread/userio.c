#include "thread/userio.h"

#include "kernel.h"
#include "libk/memory.h"
#include "mm/kalloc.h"
#include "mm/vm.h"
#include "proc/process.h"

#include <stdint.h>

void *
thread_get_stack_item(struct thread *thread, int index)
{
    void *result = NULL;
    uint32_t *stack_ptr = (uint32_t *)thread->registers.esp;

    process_switch_to_vm_area(thread->process);

    result = (void *)stack_ptr[index];

    switch_to_kernel_vm_area();

    return result;
}

int
thread_copy_from_user(struct thread *thread, void *virt, void *buf, size_t size)
{
    int res = 0;

    // Create a shared buffer between kernel and user space
    int order = kalloc_size_to_order(size);
    char *shared_buf = kalloc_get_phys_pages(order);
    if (!shared_buf) {
        return -ENOMEM;
    }

    size_t shared_buf_size = kalloc_order_to_size(order);
    memset(shared_buf, 0, shared_buf_size);

    // Map the shared buffer into user space
    void *user_shared_buf =
        vm_area_map_user_pages(thread->process->vm_area, shared_buf, shared_buf_size);
    if (!user_shared_buf) {
        res = -ENOMEM;
        goto err_out;
    }

    vm_area_switch_map(thread->process->vm_area);

    // Copy data to the shared buffer
    memcpy(user_shared_buf, virt, size);

    switch_to_kernel_vm_area();

    // Unmap the shared buffer from user space
    vm_area_unmap_pages(thread->process->vm_area, shared_buf, shared_buf_size);

    // Copy data from the shared buffer to buf
    memcpy(buf, shared_buf, size);

err_out:
    kalloc_free_phys_pages(shared_buf);

    return res;
}

int
thread_copy_to_user(struct thread *thread, void *virt, void *buf, size_t size)
{
    // TODO

    return -1;
}