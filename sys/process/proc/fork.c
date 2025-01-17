#include "proc/fork.h"

#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/memory.h"
#include "kalloc.h"
#include "vm.h"
#include "ld.h"
#include "process.h"
#include "sched.h"
#include "thread.h"

/**
 * @brief   Copy the process structure from the parent to the child.
 *
 * @param parent    The parent process.
 * @param child     The child process.
 */
static void
process_shallow_copy(const struct process *parent, struct process *child)
{
    memcpy(child, parent, sizeof(struct process));

    child->pid = process_next_pid();
    child->state = PROCESS_STATE_CREATED;
    child->vm_area = NULL;
    child->parent = NULL;
    child->children = NULL;
    child->threads = NULL;
    child->allocations = NULL;
    child->open_fds = NULL;
}

/**
 * @brief   Copy the threads from the parent process to the child process.
 *
 * @param parent    The parent process.
 * @param child     The child process.
 * @return int      Status code
 */
static int
process_copy_threads(const struct process *parent, struct process *child)
{
    struct thread *current_thread = sched_get_current();

    for_each_in_list(struct thread *, parent->threads, threads_list, thread)
    {
        struct thread *child_thread = thread_copy_to(thread, child);
        if (!child_thread) {
            return -ENOMEM;
        }

        // Set the return value on the copy of the current thread
        if (thread == current_thread) {
            thread_set_return_value(child_thread, 0);
        }

        list_push_front(&child->threads, child_thread);
    }

    return 0;
}

/**
 * @brief   Copy the allocations from the parent process to the child process.
 *
 * @param parent    The parent process.
 * @param child     The child process.
 * @return int      Status code
 */
static int
process_copy_allocations(const struct process *parent, struct process *child)
{
    for_each_in_list(struct process_allocation *, parent->allocations, alloc_list, alloc)
    {
        struct process_allocation *child_alloc = kzalloc(sizeof(struct process_allocation));
        if (!child_alloc) {
            goto err_alloc;
        }

        child_alloc->size = alloc->size;
        child_alloc->type = alloc->type;

        int order = kalloc_size_to_order(child_alloc->size);
        void *segment = kalloc_get_phys_pages(order);
        if (!segment) {
            goto err_alloc;
        }

        memcpy(segment, alloc->paddr, child_alloc->size);

        vm_area_map_pages_to(child->vm_area, alloc->vaddr, segment, segment + child_alloc->size,
                             PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER);

        child_alloc->paddr = segment;
        child_alloc->vaddr = alloc->vaddr;

        list_push_front(&child->allocations, child_alloc);
    }

    return 0;

err_alloc:
    for_each_in_list(struct process_allocation *, child->allocations, alist, allocation)
    {
        kalloc_free_phys_pages(allocation->paddr);
        kfree(alloc);
    }

    return -ENOMEM;
}

/**
 * @brief   Copy the open file descriptors from the parent process to the child process.
 *
 * @param parent    The parent process.
 * @param child     The child process.
 * @return int      Status code
 */
static int
process_copy_open_fds(const struct process *parent, struct process *child)
{
    for_each_in_list(struct process_fd *, parent->open_fds, open_fds_list, fd)
    {
        struct process_fd *child_pfd = kzalloc(sizeof(struct process_fd));
        if (!child_pfd) {
            return -ENOMEM;
        }

        memcpy(child_pfd, fd, sizeof(struct process_fd));

        list_push_front(&child->open_fds, child_pfd);
    }

    return 0;
}

int
process_fork(struct process *parent, struct process **child_out)
{
    int res = 0;

    struct process *child = kzalloc(sizeof(struct process));
    if (!child) {
        return -ENOMEM;
    }

    process_shallow_copy(parent, child);

    res = process_new_vm_area(child);
    if (res < 0) {
        goto err_vm_area;
    }

    res = process_copy_threads(parent, child);
    if (res < 0) {
        goto err_thread;
    }

    res = process_copy_allocations(parent, child);
    if (res < 0) {
        goto err_alloc;
    }

    res = process_copy_open_fds(parent, child);
    if (res < 0) {
        goto err_fds;
    }

    child->parent = parent;

    list_push_front(&parent->children, child);

    *child_out = child;
    return 0;

err_fds:
    process_free_fds(child);

err_alloc:
    process_free_allocations(child);

err_thread:
    process_free_threads(child);

err_vm_area:
    if (child->vm_area) {
        kfree(child->vm_area);
    }

    kfree(child);

    return res;
}
