#include "irq.h"

#include "config.h"
#include "errno.h"
#include "idt.h"
#include "libk/memory.h"

IRQ_HANDLER irq_handlers[LATTE_TOTAL_IDT_ENTRIES] = {0};

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

    if (irq_handlers[interrupt_no]) {
        return -EAGAIN;
    }

    irq_handlers[interrupt_no] = irq_handler;

    return 0;
}

int
do_irq(int interrupt_no)
{
    if (interrupt_no < 0 || interrupt_no > LATTE_TOTAL_IDT_ENTRIES) {
        return -EINVAL;
    }

    if (!irq_handlers[interrupt_no]) {
        return -ENOENT;
    }

    irq_handlers[interrupt_no]();

    return 0;
}
