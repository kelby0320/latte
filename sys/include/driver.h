#ifndef DRIVER_H
#define DRIVER_H

#include "fs.h"

#include <stddef.h>

#define as_device_driver(drv) ((struct device_driver *)(drv))

struct bus;
struct device;

struct device_driver {
    const char *name;
    const char *compat;
    void *private;
    struct bus *bus;
    int (*probe)(struct device *dev);
};

struct file_operations {
    int (*read)(struct device *dev, size_t offset, char *buf, size_t count);
    int (*write)(
        struct device *dev, size_t offset, const char *buf, size_t count);
    int (*seek)(struct device *dev, size_t offset, file_seek_mode_t seek_mode);
};

int
driver_init();

int
driver_register(struct device_driver *driver);

struct device_driver *
driver_find(const char *name);

struct device_driver *
driver_find_next(const struct device_driver *driver);

#endif
