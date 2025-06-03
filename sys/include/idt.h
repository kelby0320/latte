#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/**
 * @brief Interrupt Descriptor Table Register
 *
 */
struct idtr {
    // Size of the Interupt Descriptor Table - 1
    uint16_t size;

    // Base address of the IDT
    uint32_t offset;
} __attribute__((packed));

/**
 * @brief Interrupt Descriptor Table Entry
 *
 */
struct idt_entry {
    // Bits 0 - 15 of the interrupt handler address
    uint16_t offset1;

    // GDT Segment
    uint16_t segment;

    // Reserved
    uint8_t reserved;

    // Attributes - Bit 0 - 3 Gate Type, Bit 4 is 0, Bits 5 - 6 Privilege Level,
    // Bit 7 Present
    uint8_t attr;
    // Bits 16 - 31 of the interrupt handler address
    uint16_t offset2;
} __attribute__((packed));

/**
 * @brief Initialize the Interrupt Descriptor Table
 *
 */
void
idt_init();

#endif