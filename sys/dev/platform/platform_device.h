#ifndef DEV_PLATFORM_H
#define DEV_PLATFORM_H

#include "dev/device.h"

#include <stdint.h>

#define as_platform_device(dev) ((struct platform_device *)(dev))

struct device_driver;

struct resource {
    unsigned int base_addr;
    unsigned int length;
};

struct platform_device {
    struct device device;
    const char *name;
    struct list_item *resources;
};

int
platform_bus_init();

struct bus *
platform_bus_get_bus();

int
platform_match(struct device *dev, struct device_driver *drv);

int
platform_probe(struct device *dev);

int
platform_device_register(struct platform_device *pdev);

struct platform_device *
platform_device_find(const char *name);

#endif
