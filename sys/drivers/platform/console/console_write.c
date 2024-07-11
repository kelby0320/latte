#include "drivers/platform/console/console.h"

#include "dev/device.h"
#include "drivers/driver.h"
#include "drivers/platform/vga/vga.h"
#include "libk/memory.h"

/**
 * @brief Scroll the terminal downward by one line
 *
 * @param dev   Pointer to the device
 */
static void
console_scroll_screen(struct console_private *private)
{
    uint16_t *framebuffer = vga_framebuffer(private->vga);
    size_t console_height = private->max_row;
    size_t console_width = private->max_col;
    size_t console_width_bytes = console_width * sizeof(uint16_t);

    for (size_t row = 1; row < console_height; row++) {
        uint16_t *dest_row = framebuffer + (row - 1);
        uint16_t *src_row = framebuffer + row;
        memcpy(dest_row, src_row, console_width_bytes);
    }
}

/**
 * @brief Output a newline to the terminal
 *
 * @param dev   Pointer to the device
 */
static void
console_newline(struct console_private *private)
{
    if (private->cur_row == private->max_row - 1) {
        console_scroll_screen(private);
        private->cur_col = 0;
        return;
    }

    private->cur_row++;
    private->cur_col = 0;
}

/**
 * @brief Output a character to the terminal
 *
 * @param dev   Pointer to the device
 * @param c             Character to output
 */
static void
console_put_char(struct console_private *private, char c)
{
    uint16_t ch = vga_make_char(private->vga, c);

    vga_set_char(private->vga, private->cur_row, private->cur_col, ch);

    private->cur_col++;

    if (private->cur_col >= private->max_col) {
        console_newline(private);
    }
}



/**
 * @brief Handle an input character
 *
 * @param dev   Pointer to the device
 * @param c     Character to handle
 */
static void
console_handle_char(struct console_private *private, char c)
{ 
    if (c == '\n') {
        console_newline(private);
        return;
    }

    console_put_char(private, c);
}

int
console_write(struct device *dev, size_t offset, const char *buf, size_t count)
{
    struct device_driver *drv = dev->driver;
    struct console_private *private = drv->private;

    for (size_t i = 0; i < count; i++) {
	console_handle_char(private, buf[i]);
    }

    return count;
}

int
console_clear_screen(struct device *dev)
{
    struct device_driver *drv = dev->driver;
    struct console_private *private = drv->private;
    
    size_t console_height = private->max_row;
    size_t console_width = private->max_col;

    for (size_t y = 0; y < console_height; y++) {
        for (size_t x = 0; x < console_width; x++) {
            uint16_t ch = vga_make_char(private->vga, ' ');
            vga_set_char(private->vga, x, y, ch);
        }
    }
}
