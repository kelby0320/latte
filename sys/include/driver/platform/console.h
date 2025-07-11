#ifndef DRIVERS_CONSOLE_H
#define DRIVERS_CONSOLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CONSOLE_INPUT_BUFFER_SIZE 512

struct device;
struct platform_device;
struct input_device;

struct console_private {
    struct platform_device *console_device;

    struct device *vga;
    uint16_t cur_col;
    uint16_t cur_row;
    uint16_t max_col;
    uint16_t max_row;
    uint8_t color;

    struct input_device *kbd_idev;
    char input_buffer[CONSOLE_INPUT_BUFFER_SIZE];
    unsigned int input_buffer_len;
    bool input_ready;
    bool shift_enabled;
    bool capslock_enabled;
};

int
console_drv_init();

int
console_probe(struct platform_device *dev);

int
console_write(struct device *dev, size_t offset, const char *buf, size_t count);

int
console_read(struct device *dev, size_t offset, char *buf, size_t count);

void
console_clear_screen(struct device *dev);

#endif
