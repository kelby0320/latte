#ifndef PLATFORM_DRIVER_H
#define PLATFORM_DRIVER_H

#include "drivers/driver.h"

#define as_platform_driver(drv) ((struct platform_driver *)(drv))

struct platform_device;

struct platform_driver {
    struct device_driver driver;
    void *private;
    int (*probe)(struct platform_device *pdev);
};

int
platform_driver_register(struct platform_driver *driver);

#endif
