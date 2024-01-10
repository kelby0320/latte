#ifndef DEVICE_H
#define DEVICE_H

#include "config.h"

#include <stddef.h>

struct bus;
struct device;

enum { SEEK_SET, SEEK_CUR, SEEK_END };

/**
 * @brief File operations structure for devices
 *
 */
struct file_operations {
    int (*open)(struct device *device);
    int (*close)(struct device *device);
    int (*read)(struct device *device, char *buf, size_t count);
    int (*write)(struct device *device, const char *buf, size_t count);
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

    // Pointer to private data for the device
    void *private;
};

int
device_add_device(struct device *device);

#endif