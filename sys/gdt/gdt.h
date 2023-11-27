#ifndef GDT_H
#define GDT_H

#include <stddef.h>
#include <stdint.h>

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
 * @brief Structured GDT item
 * 
 */
struct gdt_structured {
    uint32_t base;
    uint32_t limit;
    uint8_t type;
};

/**
 * @brief Load the global descriptor table
 * 
 * Defined in gdt.S
 * 
 * @param gdt   Pointer to the GDT
 * @param size  Size of the GDT
 */
void
gdt_load(struct gdt *gdt, size_t size);

/**
 * @brief Convert structure gdt to real gdt
 * 
 * @param gdt               Pointer to real gdt
 * @param gdt_structured    Ponter to structure gdt
 * @param total_entries     Total entries in gdt
 */
void
gdt_structured_to_gdt(struct gdt *gdt, struct gdt_structured *gdt_structured, int total_entries);

#endif
