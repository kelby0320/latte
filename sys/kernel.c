#include "config.h"
#include "kernel.h"
#include "gdt/gdt.h"
#include "libk/memory.h"

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY (uint16_t*)0xB8000

static uint16_t *terminal_buffer = NULL;
static size_t terminal_row = 0;
static size_t terminal_col = 0;
static uint8_t terminal_color;

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15
};

static inline uint8_t
vga_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t
terminal_makechar(char c)
{
    return (terminal_color << 8) | c;
}

static void
terminal_putchar(size_t col, size_t row, char c)
{
    terminal_buffer[(row * VGA_WIDTH) + col] = terminal_makechar(c);
}

static void
terminal_writechar(char c)
{
    terminal_putchar(terminal_col, terminal_row, c);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH) {
	terminal_col = 0;
	terminal_row++;
    }
}

static void
terminal_initialize()
{
    terminal_buffer = VGA_MEMORY;
    terminal_row = 0;
    terminal_col = 0;
    terminal_color = vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
	for (size_t x = 0; x < VGA_WIDTH; x++) {
	    terminal_putchar(x, y, ' ');
	}
    }
}

static size_t
strlen(const char *str)
{
    size_t count = 0;
    
    while (*str) {
	count++;
	str++;
    }

    return count;
}

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
    
    print("Latte OS v0.1");

    while (1);
}
