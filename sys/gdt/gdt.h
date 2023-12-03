#ifndef GDT_H
#define GDT_H

#include <stddef.h>
#include <stdint.h>

struct tss;

/**
 * @brief Global Descriptor Table Descriptor
 * 
 */
struct gdtr {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

/**
 * @brief Global Descriptor Table item structure
 * 
 */
struct gdt {
    uint16_t limit;
    uint16_t base_0_15;
    uint8_t base_16_23;
    uint8_t access;
    uint8_t flags_limit;
    uint8_t base_24_31;
} __attribute__((packed));

/**
 * @brief Initialize the Global Descriptor Table
 * 
 */
void
gdt_init();

/**
 * @brief Set the TSS entry of the GDT
 * 
 * @param tss 
 */
void
gdt_set_tss(struct tss *tss);

/**
 * @brief Sets the segment registers to the kernel data segment
 * 
 */
void
gdt_set_kernel_data_segment();

/**
 * @brief Sets the segment registers to the user data segment
 * 
 */
void
gdt_set_user_data_segment();

#endif
