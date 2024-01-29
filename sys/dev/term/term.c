#include "dev/term/term.h"

#include "dev/vga/vga.h"
#include "errno.h"
#include "libk/memory.h"
#include "libk/string.h"

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Make a character by combining the terminal device's character color byte with a character
 *
 * @param term_device   Pointer to the device
 * @param c             Character to combine with the terminals color byte
 * @return uint16_t     Output character
 */
static uint16_t
term_device_make_char(struct term_device *term_device, char c)
{
    return (term_device->char_color << 8) | c;
}

/**
 * @brief Set the value of a character on the terminal
 *
 * @param term_device   Pointer to the device
 * @param row           Row number
 * @param col           Column number
 * @param c             Character to set
 */
static void
term_device_set_char(struct term_device *term_device, size_t row, size_t col, uint16_t c)
{
    term_device->framebuffer[(row * term_device->term_width) + col] = c;
}

/**
 * @brief Scroll the terminal downward by one line
 *
 * @param term_device   Pointer to the device
 */
static void
term_device_scroll_screen(struct term_device *term_device)
{
    uint16_t *framebuffer = term_device->framebuffer;
    size_t term_height = term_device->term_height;
    size_t term_width = term_device->term_width;
    size_t term_width_bytes = term_width * sizeof(uint16_t);

    for (size_t row = 1; row < term_height; row++) {
        uint16_t *dest_row = framebuffer + (row - 1);
        uint16_t *src_row = framebuffer + row;
        memcpy(dest_row, src_row, term_width_bytes);
    }
}

/**
 * @brief Output a newline to the terminal
 *
 * @param term_device   Pointer to the device
 */
static void
term_device_newline(struct term_device *term_device)
{
    if (term_device->current_row == term_device->term_height - 1) {
        term_device_scroll_screen(term_device);
        term_device->current_col = 0;
        return;
    }

    term_device->current_row++;
    term_device->current_col = 0;
}

/**
 * @brief Output a character to the terminal
 *
 * @param term_device   Pointer to the device
 * @param c             Character to output
 */
static void
term_device_put_char(struct term_device *term_device, char c)
{
    uint16_t ch = term_device_make_char(term_device, c);

    term_device_set_char(term_device, term_device->current_row, term_device->current_col, ch);

    term_device->current_col++;

    if (term_device->current_col >= term_device->term_width) {
        term_device_newline(term_device);
    }
}

/**
 * @brief Handle an input character
 *
 * @param term_device   Pointer to the device
 * @param c             Character to handle
 */
static void
term_device_handle_char(struct term_device *term_device, char c)
{
    if (c == '\n') {
        term_device_newline(term_device);
        return;
    }

    term_device_put_char(term_device, c);
}

/**
 * @brief Terminal device write file operation
 *
 * @param device    Pointer to the device
 * @param offset    Write offset (ignored)
 * @param buf       Buffer to output
 * @param count     Number of characters to write
 * @return int      Number of characters written
 */
static int
term_device_fops_write(struct device *device, size_t offset, const char *buf, size_t count)
{
    struct term_device *term_device = (struct term_device *)device;

    for (size_t i = 0; i < count; i++) {
        term_device_handle_char(term_device, buf[i]);
    }

    return count;
}

int
term_device_init(struct term_device *term_device)
{
    strcpy(term_device->device.name, "term0");

    struct vga_device *vga_device = (struct vga_device *)device_find("vga0");
    if (!vga_device) {
        return -EEXIST;
    }

    term_device->vga_device = vga_device;
    term_device->framebuffer = vga_device_get_framebuffer(vga_device);

    term_device->current_row = 0;
    term_device->current_col = 0;
    term_device->term_width = VGA_WIDTH;
    term_device->term_height = VGA_HEIGHT;
    term_device->char_color = vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    term_device->device.file_operations.write = term_device_fops_write;

    term_device_clear_screen(term_device);

    return 0;
}

int
term_device_write(struct term_device *term_device, const char *buf, size_t count)
{
    return term_device->device.file_operations.write((struct device *)term_device, 0, buf, count);
}

void
term_device_clear_screen(struct term_device *term_device)
{
    size_t term_height = term_device->term_height;
    size_t term_width = term_device->term_width;

    for (size_t y = 0; y < term_height; y++) {
        for (size_t x = 0; x < term_width; x++) {
            uint16_t ch = term_device_make_char(term_device, ' ');
            term_device_set_char(term_device, x, y, ch);
        }
    }
}