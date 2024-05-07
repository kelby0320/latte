#include "thread/thread.h"

#include "config.h"
#include "errno.h"
#include "irq/isr.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/memory.h"
#include "libk/print.h"
#include "mm/kalloc.h"
#include "mm/paging/paging.h"
#include "mm/vm.h"
#include "proc/process.h"
#include "sched/sched.h"

#include <stddef.h>

extern struct vm_area kernel_vm_area;

struct list_item *thread_list = NULL;
static uint32_t next_tid = 1;

/**
 * @brief Get the next tid
 *
 * @return uint32_t
 */
static inline uint32_t
get_next_tid()
{
    return next_tid++;
}

/**
 * @brief Setup processor regisers and begin executing in user space
 *
 * @param registers Pointer to registers structure
 */
void
thread_return(struct registers *registers);

int
thread_create(struct process *process)
{
    struct thread *thread = kzalloc(sizeof(struct thread));
    if (!thread) {
        return -ENOMEM;
    }

    int stack_order = kalloc_size_to_order(THREAD_STACK_SIZE);
    void *stack = kalloc_get_phys_pages(stack_order);
    if (!stack) {
        return -ENOMEM;
    }

    // Map the stack from the bottom up.
    // Note: We need to map one extra page to cover the top of stack address
    int res = vm_area_map_pages_to(process->vm_area, (void *)THREAD_STACK_VIRT_ADDR_BOTTOM, stack,
                                   stack + THREAD_STACK_SIZE + PAGING_PAGE_SIZE,
                                   PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE | PAGING_PAGE_USER);
    if (res < 0) {
        kalloc_free_phys_pages(stack);
        goto err_out;
    }

    thread->state = THREAD_STATE_NEW;
    thread->tid = get_next_tid();
    memset(&thread->registers, 0, sizeof(struct registers));
    thread->registers.ip = process->elf_img_desc.elf_header.e_entry;
    thread->registers.cs = LATTE_USER_CODE_SEGMENT;
    thread->registers.ds = LATTE_USER_DATA_SEGMENT;
    thread->registers.ss = LATTE_USER_DATA_SEGMENT;
    thread->registers.esp = THREAD_STACK_VIRT_ADDR_TOP;
    thread->stack = (void *)THREAD_STACK_VIRT_ADDR_TOP;
    thread->stack_size = THREAD_STACK_SIZE;
    thread->process = process;

    list_push_front(&thread_list, thread);

    sched_add_thread(thread);

    return thread->tid;

err_out:
    kfree(thread);
    return res;
}

struct thread *
thread_get(uint32_t tid)
{
    for_each_in_list(struct thread *, thread_list, list, thread)
    {
        if (thread->tid == tid) {
            return thread;
        }
    }

    return NULL;
}

void
thread_destroy(struct thread *thread)
{
    int res = sched_remove_thread(thread);
    if (res < 0) {
        printk("Error unscheduling thread %d", thread->tid);
    }

    res = list_remove(&thread_list, thread);
    if (res < 0) {
        printk("Error removing thread %d", thread->tid);
    }

    kalloc_free_phys_pages(thread->stack);

    kfree(thread);
}

void
thread_set_return_value(struct thread *thread, uint32_t value)
{
    thread->registers.eax = value;
}

void
thread_switch_and_return(struct thread *thread)
{
    paging_copy_kernel_pages_to_user(kernel_vm_area.page_directory,
                                     thread->process->vm_area->page_directory);
    process_switch_to_vm_area(thread->process);
    thread_return(&thread->registers);
}

void
thread_save_state(struct thread *thread, struct isr_frame *isr_frame)
{
    thread->registers.edi = isr_frame->edi;
    thread->registers.esi = isr_frame->esi;
    thread->registers.ebp = isr_frame->ebp;
    thread->registers.ebx = isr_frame->ebx;
    thread->registers.edx = isr_frame->edx;
    thread->registers.ecx = isr_frame->ecx;
    thread->registers.eax = isr_frame->eax;
    thread->registers.ip = isr_frame->ip;
    thread->registers.cs = isr_frame->cs;
    thread->registers.flags = isr_frame->flags;
    thread->registers.esp = isr_frame->esp;
    thread->registers.ss = isr_frame->ss;
}