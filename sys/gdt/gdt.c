#include "gdt.h"

#include "config.h"
#include "libk/memory.h"
#include "kernel.h"

struct gdtr gdtr;
struct gdt gdt[LATTE_TOTAL_GDT_SEGMENTS];

/**
 * @brief Structured GDT item
 * 
 */
struct gdt_structured {
    uint32_t base;
    uint32_t limit;
    uint8_t type;
};

struct gdt_structured gdt_structured[LATTE_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00}, /* Null Segment */
    {.base = 0x00, .limit = 0xFFFFF, .type = 0x9A}, /* Kernel Code Segment */
    {.base = 0x00, .limit = 0xFFFFF, .type = 0x92}, /* Kernel Data Segment */
    {.base = 0x00, .limit = 0xFFFFF, .type = 0xF8}, /* User Code Segment */
    {.base = 0x00, .limit = 0xFFFFF, .type = 0xF2}, /* User Data Segment */
};

/**
 * @brief Load the Global Descriptor Table
 * 
 * Defined in gdt.S
 * 
 * @param gdtr   Pointer to the GDT descriptor structrue
 */
void
load_gdt(struct gdtr *gdtr);

/**
 * @brief Encode structure gdt as real gdt
 * 
 * @param gdt               Pointer to read gdt item
 * @param gdt_structured    Ponter to structure gdt item
 */
static void
encode_gdt_structured(struct gdt* gdt, const struct gdt_structured *gdt_structured)
{
    if (gdt_structured->limit > 0xFFFFF) {
	    panic("GDT cannot encode limits greater than 0xFFFFF\n");
    }

    // Encode the limit
    gdt->limit = gdt_structured->limit & 0xFFFF;
    gdt->flags_limit = (gdt_structured->limit >> 16) & 0x0F;

    // Encode the base
    gdt->base_0_15 = gdt_structured->base & 0xFFFF;
    gdt->base_16_23 = (gdt_structured->base >> 16) & 0xFF;
    gdt->base_24_31 = (gdt_structured->base >> 24) & 0xFF;

    // Encode access byte
    gdt->access = gdt_structured->type;

    // Encode flags
    // Set G flag for page granularity
    // Set DB flag for 32 bit protected mode
    gdt->flags_limit = gdt->flags_limit | 0b11000000;
}

/**
 * @brief Convert structure gdt to real gdt
 * 
 * @param gdt               Pointer to real gdt
 * @param gdt_structured    Ponter to structure gdt
 * @param total_entries     Total entries in gdt
 */
static void
gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* structured_gdt, int total_entries)
{
    for (int i = 0; i < total_entries; i++) {
	    encode_gdt_structured(&gdt[i], &structured_gdt[i]);
    }
}

void
gdt_init()
{
    memset(gdt, 0, sizeof(gdt));
    memset(&gdtr, 0, sizeof(struct gdtr));
    gdt_structured_to_gdt(gdt, gdt_structured, LATTE_TOTAL_GDT_SEGMENTS);
    gdtr.size = sizeof(gdt);
    gdtr.offset = gdt;
    load_gdt(&gdtr);
}
