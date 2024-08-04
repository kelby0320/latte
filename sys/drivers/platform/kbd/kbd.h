#ifndef DRIVERS_KBD_H
#define DRIVERS_KBD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct device;
struct input_device;

struct kbd_private {
    bool caps_lock;
    unsigned int *scancode_set;
    struct input_device *idev;
};

int
kbd_drv_init();

int
kbd_probe(struct device *dev);

int
kbd_read(struct device *dev, char *buf, size_t len);

#endif
