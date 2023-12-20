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

void
irq_init()
{
    idt_init();
}

int
register_irq(int interrupt_no, IRQ_HANDLER irq_handler)
{
    if (interrupt_no < 0 || interrupt_no > LATTE_TOTAL_IDT_ENTRIES) {
        return -EINVAL;
    }

    idt_set_entry(interrupt_no, irq_handler);
}
