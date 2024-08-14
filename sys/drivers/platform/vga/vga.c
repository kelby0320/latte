#include "drivers/platform/vga/vga.h"

#include "cpu/port.h"
#include "dev/device.h"
#include "dev/platform/platform_device.h"
#include "drivers/driver.h"
#include "drivers/platform/platform_driver.h"
#include "errno.h"
#include "libk/alloc.h"

#include <stdint.h>

#define VGA_FRAMEBUFFER_ADDRESS (uint16_t *)0xB8000
#define VGA_FB_HEIGHT 24
#define VGA_FB_WIDTH 80

struct platform_driver vga_drv = {
    .driver = {
	.name = "vga",
	.compat = "vga"
    },
    .probe = vga_probe
};

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

int
vga_drv_init()
{
    platform_driver_register(&vga_drv);

    return 0;
}

int
vga_probe(struct platform_device *pdev)
{
    struct vga_private *private = kzalloc(sizeof(struct vga_private));
    if (!private) {
	return -ENOMEM;
    }

    private->framebuffer = VGA_FRAMEBUFFER_ADDRESS;
    private->fb_height = VGA_FB_HEIGHT;
    private->fb_width = VGA_FB_WIDTH;
    private->cur_char_color = vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    pdev->device.driver->private = private;

    vga_enable_cursor();
    vga_set_cursor(as_device(pdev), 0, 0);

    return 0;
}

uint16_t *
vga_framebuffer(struct device *dev)
{
    struct vga_private *private = dev->driver->private;
    return private->framebuffer;   
}

uint16_t
vga_make_char(struct device *dev, char ch)
{
    struct vga_private *private = dev->driver->private;
    return (private->cur_char_color << 8) | ch;
}

void
vga_set_char(struct device *dev, uint16_t row, uint16_t col, uint16_t ch)
{
    struct vga_private *private = dev->driver->private;
    private->framebuffer[(row * private->fb_width) + col] = ch;
}

uint16_t
vga_max_column(struct device *dev)
{
    struct vga_private *private = dev->driver->private;
    return private->fb_width;
}

uint16_t
vga_max_row(struct device *dev)
{
    struct vga_private *private = dev->driver->private;
    return private->fb_height; 
}

uint8_t
vga_current_color(struct device *dev)
{
    struct vga_private *private = dev->driver->private;
    return private->cur_char_color;
}

void
vga_enable_cursor()
{
    uint8_t cursor_start = 14;
    uint8_t cursor_end = 15;
    
    outb(0x3D4, 0x0A);
    outb(0x3D5, (insb(0x3D5) & 0xC0) | cursor_start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (insb(0x3D5) & 0xE0) | cursor_end);
}

void
vga_disable_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void
vga_set_cursor(struct device *dev, uint16_t row, uint16_t col)
{
    struct vga_private *private = dev->driver->private;
    uint16_t cursor_pos = row * private->fb_width + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(cursor_pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((cursor_pos >> 8) & 0xFF));
}
