#ifndef TERM_H
#define TERM_H

#include "dev/device.h"

struct vga_device;

/**
 * @brief
 *
 */
struct term_device {
    // Base device structure
    struct device device;

    // Pointer to underlying VGA device
    struct vga_device *vga_device;

    // Pointer to the VGA framebuffer memory
    uint16_t *framebuffer;

    // Terminal width in columns
    size_t term_width;

    // Terminal height in rows
    size_t term_height;

    // Current row position
    size_t current_row;

    // Current column position
    size_t current_col;

    // Character color
    uint8_t char_color;
};

/**
 * @brief Initialize a terminal device
 *
 * @param term_device   Pointer to the device
 * @return int          Status code
 */
int
term_device_init(struct term_device *term_device);

/**
 * @brief Write to the terminal
 *
 * @param term_device   Pointer to the device
 * @param buf           Buffer to write
 * @param count         Number of characters to write
 * @return int          Number of characters written
 */
int
term_device_write(struct term_device *term_device, const char *buf, size_t count);

/**
 * @brief Clear the terminal
 *
 * @param term_device   Pointer to the device
 */
void
term_device_clear_screen(struct term_device *term_device);

#endif