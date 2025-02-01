#include "idt.h"

#include "config.h"
#include "errno.h"
#include "isr.h"
#include "libk/memory.h"
#include "libk/print.h"

#include <stdint.h>

struct idtr idtr;
struct idt_entry int_desc_tbl[LATTE_TOTAL_IDT_ENTRIES];

extern void *isr_wrapper_table[256];

extern void
isr_syscall_wrapper();

/**
 * @brief Load the interrupt descriptor table
 *
 */
void
load_idt(struct idtr *idtr);

/**
 * @brief Set an entry in the IDT
 *
 * @param interrupt_no  Interrupt Number
 * @param isr           Interrupt Handler function pointer
 * @return int
 */
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
    printk("Init IDT at %d\n", (int)int_desc_tbl);

    memset(int_desc_tbl, 0, sizeof(int_desc_tbl));
    idtr.size = sizeof(int_desc_tbl) - 1;
    idtr.offset = (uint32_t)int_desc_tbl;

    for (int i = 0; i < LATTE_TOTAL_IDT_ENTRIES; i++) {
        idt_set_entry(i, isr_wrapper_table[i]);
    }

    idt_set_entry(0x80, isr_syscall_wrapper);

    load_idt(&idtr);

    printk("Loaded Interrupt Descriptor Table\n");
}
