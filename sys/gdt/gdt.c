#include "gdt.h"
#include "kernel.h"

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

void
gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* structured_gdt, int total_entries)
{
    for (int i = 0; i < total_entries; i++) {
	    encode_gdt_structured(&gdt[i], &structured_gdt[i]);
    }
}
