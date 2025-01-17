#include "term.h"

#include "libk/string.h"
#include "libk/memory.h"

#include <stdint.h>

#define VGA_FRAMEBUFFER    (uint16_t *)0XB8000
#define VGA_HEIGHT         24
#define VGA_WIDTH          80
#define VGA_CHAR_COLOR     (uint16_t)(0x0F)

static uint16_t row = 0;
static uint16_t col = 0;

static uint16_t
vga_char(char c)
{
    return (VGA_CHAR_COLOR << 8) | c;
}

static void
set_char(uint16_t row, uint16_t col, char c)
{
   uint16_t vchar = vga_char(c);
   uint16_t *fb = VGA_FRAMEBUFFER;
   
   fb[(row * VGA_WIDTH) + col] = vchar;
}

static void
term_scroll()
{
    uint16_t *fb = VGA_FRAMEBUFFER;
    
    for (size_t row = 1; row <= VGA_HEIGHT; row++) {
        uint16_t *dest_row = fb + ((row - 1) * VGA_WIDTH);
        uint16_t *src_row = fb + (row * VGA_WIDTH);
        memcpy(dest_row, src_row, VGA_WIDTH  * 2);
    }

    uint16_t ch = vga_char(' ');
    for (int i = 0; i < VGA_WIDTH; i++) {
	set_char(VGA_HEIGHT, i, ch);
    }
}

static void
term_newline()
{
    if (row == VGA_HEIGHT) {
	term_scroll();
	col = 0;
	return;
    }

    row++;
    col = 0;
}

static void
term_put_char(char c)
{
    if (c == '\n') {
	term_newline();
	return;
    }

    set_char(row, col, c);

    col++;
    if (col > VGA_WIDTH) {
	term_newline();
    }
}

void
term_init()
{
    term_clear_screen();
}

void
term_clear_screen()
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            uint16_t ch = vga_char(' ');
            set_char(y, x, ch);
        }
    }
}

int
term_write(const char *str)
{
    int len = strlen(str);
    
    for (int i = 0; i < len; i++) {
	term_put_char(str[i]);
    }

    return len;
}
