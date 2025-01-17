#include "isr.h"

#include "port.h"
#include "irq.h"
#include "kernel.h"
#include "vm.h"
#include "process.h"
#include "sched.h"
#include "syscall.h"
#include "thread.h"
#include "libk/print.h"

static void
ack_int(int interrupt_no)
{
    if (interrupt_no >= 0x28) {
	outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);
}

void
isr_handler_wrapper(int interrupt_no, struct isr_frame *isr_frame)
{
    switch_to_kernel_vm_area();

    struct thread *current_thread = sched_get_current();
    if (current_thread) {
	thread_save_state(current_thread, isr_frame);
    }

    do_irq(interrupt_no);

    ack_int(interrupt_no);

    if (current_thread) {
	schedule(); // No return
    }
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
