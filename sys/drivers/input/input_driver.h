#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H

#include "drivers/driver.h"

#define as_input_driver(drv) ((struct input_driver *)(drv))

struct input_device;

struct input_driver {
    struct device_driver driver;
    void *private;
    int (*probe)(struct device *dev);
};

int
input_driver_init();

int
input_driver_register(struct input_driver *driver);

#endif
