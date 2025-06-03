#ifndef PLATFORM_DRIVER_H
#define PLATFORM_DRIVER_H

#include "driver.h"

#define as_platform_driver(drv) ((struct platform_driver *)(drv))

struct platform_device;

struct platform_driver {
    struct device_driver driver;
    void *private;
    int (*probe)(struct platform_device *dev);
};

int
platform_driver_init();

int
platform_driver_register(struct platform_driver *driver);

#endif
