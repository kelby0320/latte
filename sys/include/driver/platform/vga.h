#ifndef DRIVERS_VGA_H
#define DRIVERS_VGA_H

#include <stdint.h>

struct device;
struct platform_device;

/**
 * @brief Standard VGA colors
 *
 */
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

struct vga_private {
    uint16_t *framebuffer;
    uint16_t fb_height;
    uint16_t fb_width;
    uint8_t cur_char_color;
};

int
vga_drv_init();

int
vga_probe(struct platform_device *pdev);

uint16_t *
vga_framebuffer(struct device *dev);

uint16_t
vga_make_char(struct device *dev, char ch);

void
vga_set_char(struct device *dev, uint16_t row, uint16_t col, uint16_t ch);

uint16_t
vga_max_column(struct device *dev);

uint16_t
vga_max_row(struct device *dev);

uint8_t
vga_current_color(struct device *dev);

void
vga_enable_cursor();

void
vga_disable_cursor();

void
vga_set_cursor(struct device *dev, uint16_t row, uint16_t col);

#endif
