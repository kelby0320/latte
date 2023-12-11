#include "dev/term/term.h"

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY (uint16_t *)0xB8000

static uint16_t *terminal_buffer = NULL;
static size_t    terminal_row = 0;
static size_t    terminal_col = 0;
static uint8_t   terminal_color;

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

void
terminal_writechar(char c)
{
    if (c == '\n') {
        terminal_row++;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c);

    terminal_col++;
    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
    }
}

void
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