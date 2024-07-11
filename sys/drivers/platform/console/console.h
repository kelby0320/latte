#ifndef DRIVERS_CONSOLE_H
#define DRIVERS_CONSOLE_H

#include <stddef.h>
#include <stdint.h>

struct device;
struct platform_device;


struct console_private {
    struct device *vga;
    uint16_t cur_col;
    uint16_t cur_row;
    uint16_t max_col;
    uint16_t max_row;
    uint8_t color;
};

int
console_drv_init();

int
console_probe(struct platform_device *pdev);

int
console_write(struct device *dev, size_t offset, const char *buf, size_t count);

int
console_clear_screen(struct device *dev);

#endif
