#ifndef DEV_DEVICE_H
#define DEV_DEVICE_H

#include <stdint.h>

#define as_device(dev) ((struct device *)(dev))

struct bus;
struct device_driver;

struct device {
    char *name;
    unsigned int id;
    struct bus *bus;
    struct device_driver *driver;
};

int
make_device(struct device *dev, const char *name);

#endif
