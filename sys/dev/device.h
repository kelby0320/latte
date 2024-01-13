#ifndef DEVICE_H
#define DEVICE_H

#include "config.h"
#include "fs/fs.h"

#include <stddef.h>

struct bus;
struct device;

/**
 * @brief File operations structure for devices
 *
 */
struct file_operations {
    int (*open)(struct device *device);
    int (*close)(struct device *device);
    int (*read)(struct device *device, size_t offset, char *buf, size_t count);
    int (*write)(struct device *device, size_t offset, const char *buf, size_t count);
    int (*seek)(struct device *device, int offset, unsigned int seek_mode);
};

/**
 * @brief Device structure
 *
 */
struct device {
    // Name of the device
    char name[LATTE_DEVICE_NAME_MAX_SIZE];

    // Device id
    // Note: this is not a unique value
    unsigned int id;

    // Pointer to the bus the device is on
    struct bus *bus;

    // Pointer to file_operations structure for the device
    struct file_operations file_operations;
};

int
device_add_device(struct device *device);

struct device *
device_find(const char *name);

#endif