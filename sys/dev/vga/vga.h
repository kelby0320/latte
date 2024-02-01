#ifndef VGA_H
#define VGA_H

#include "dev/device.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

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

/**
 * @brief
 *
 */
struct vga_device {
    // Base device structure
    struct device device;
};

#define as_vga_device(ptr) ((struct vga_device *)ptr)

/**
 * @brief
 *
 * @param vga_device
 * @return int
 */
int
vga_device_init(struct vga_device *vga_device);

/**
 * @brief
 *
 * @param vga_device
 * @return int
 */
int
vga_device_set_mode_3(struct vga_device *vga_device);

/**
 * @brief
 *
 * @param vga_device
 * @return int
 */
int
vga_device_set_mode_13(struct vga_device *vga_device);

/**
 * @brief
 *
 * @param vga_device
 * @return void*
 */
void *
vga_device_get_framebuffer(struct vga_device *vga_device);

/**
 * @brief Make a VGA color byte
 *
 * @param fg        Foreground color
 * @param bg        Background color
 * @return uint8_t  Color byte
 */
static inline uint8_t
vga_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

#endif