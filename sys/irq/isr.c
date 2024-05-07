#include "irq/isr.h"

#include "irq/irq.h"
#include "kernel.h"
#include "mm/vm.h"
#include "port/io.h"
#include "proc/process.h"
#include "sched/sched.h"
#include "syscall/syscall.h"
#include "thread/thread.h"

void
isr_handler_wrapper(int interrupt_no, struct isr_frame *isr_frame)
{
    switch_to_kernel_vm_area();

    struct thread *current_thread = sched_get_current();
    thread_save_state(current_thread, isr_frame);

    do_irq(interrupt_no);

    vm_area_switch_map(current_thread->process->vm_area);

    outb(0x20, 0x20);
}

void
isr_syscall_handler_wrapper(int syscall_no, struct isr_frame *isr_frame)
{
    switch_to_kernel_vm_area();

    struct thread *current_thread = sched_get_current();
    thread_save_state(current_thread, isr_frame);

    do_syscall(syscall_no);

    schedule(); // No return
}