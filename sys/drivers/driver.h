#ifndef DRIVER_H
#define DRIVER_H

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

int
driver_init();

int
driver_register(struct device_driver *driver);

struct device_driver *
driver_find(const char *name);

struct device_driver *
driver_find_next(const struct device_driver *driver);

#endif
