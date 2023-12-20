#include "irq/idt.h"

#include "config.h"
#include "errno.h"
#include "irq/irq.h"
#include "kernel.h"
#include "libk/memory.h"

#include <stdint.h>

extern void *isr_table[LATTE_TOTAL_IDT_ENTRIES];

struct idtr idtr;
struct idt_entry int_desc_tbl[LATTE_TOTAL_IDT_ENTRIES];

/**
 * @brief Load the interrupt descriptor table
 *
 * Defined in idt.S
 *
 */
void
load_idt(struct idtr *idtr);

int
idt_set_entry(int interrupt_no, void *isr)
{
    if (interrupt_no < 0 || interrupt_no > LATTE_TOTAL_IDT_ENTRIES) {
        return -EINVAL;
    }

    struct idt_entry *idt_entry = &int_desc_tbl[interrupt_no];
    idt_entry->offset1 = (uint32_t)isr & 0x0000ffff;
    idt_entry->segment = LATTE_KERNEL_CODE_SEGMENT;
    idt_entry->reserved = 0;
    idt_entry->attr = 0xee;
    idt_entry->offset2 = (uint32_t)isr >> 16;

    return 0;
}

void
idt_init()
{
    memset(int_desc_tbl, 0, sizeof(int_desc_tbl));
    idtr.size = sizeof(int_desc_tbl) - 1;
    idtr.offset = (uint32_t)int_desc_tbl;

    load_idt(&idtr);
}