#include "config.h"
#include "dev/disk/disk.h"
#include "dev/term/term.h"
#include "fs/fs.h"
#include "gdt/gdt.h"
#include "kernel.h"
#include "libk/libk.h"
#include "libk/memory.h"
#include "libk/string.h"

void
print(const char *str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
	    terminal_writechar(str[i]);
    }
}

void
panic(const char *str)
{
    print(str);
    while (1);
}

struct gdt gdt[LATTE_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[LATTE_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00}, /* Null Segment */
    {.base = 0x00, .limit = 0xFFFFF, .type = 0x9A}, /* Kernel Code Segment */
    {.base = 0x00, .limit = 0xFFFFF, .type = 0x92}, /* Kernel Data Segment */
    {.base = 0x00, .limit = 0xFFFFF, .type = 0xF8}, /* User Code Segment */
    {.base = 0x00, .limit = 0xFFFFF, .type = 0xF2}, /* User Data Segment */
};

void
kernel_main()
{
    terminal_initialize();

    // Initialize GDT
    memset(gdt, 0, sizeof(gdt));
    gdt_structured_to_gdt(gdt, gdt_structured, LATTE_TOTAL_GDT_SEGMENTS);

    // Load the GDT
    gdt_load(gdt, sizeof(gdt));

    // Initialize libk
    libk_init();

    // Initialize filesystem drivers
    fs_init();

    // Find and Initialize Disks
    disk_probe_and_init();
    
    print("Latte OS v0.1");

    while (1);
}
