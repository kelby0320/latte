#include "irq/irq.h"

#include "config.h"
#include "errno.h"
#include "irq/idt.h"
#include "kernel.h"
#include "libk/memory.h"
#include "port/io.h"
#include "task/process.h"
#include "task/sched.h"
#include "task/task.h"

static IRQ_HANDLER irq_handlers[LATTE_TOTAL_IDT_ENTRIES];

/**
 * @brief
 *
 * @param interrupt_no
 * @param irq_frame
 */
void
generic_isr_handler(int interrupt_no, struct irq_frame *irq_frame)
{
    switch_to_kernel_vm_area();
    struct task *current_task = sched_get_current();

    if (irq_handlers[interrupt_no]) {
        task_save_state(current_task, irq_frame);
        irq_handlers[interrupt_no]();
    }

    process_switch_to_vm_area(current_task->process);
    outb(0x20, 0x20);
}

void
irq_init()
{
    memset(irq_handlers, 0, sizeof(irq_handlers));
    idt_init();
}

int
register_irq(int interrupt_no, IRQ_HANDLER irq_handler)
{
    if (interrupt_no < 0 || interrupt_no > LATTE_TOTAL_IDT_ENTRIES) {
        return -EINVAL;
    }

    irq_handlers[interrupt_no] = irq_handler;
}
