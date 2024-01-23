#ifndef DEVICE_H
#define DEVICE_H

#include "config.h"
#include "fs/fs.h"

#include <stddef.h>

#define DEVICE_TYPE_RAW   0
#define DEVICE_TYPE_BLOCK 1

struct bus;
struct device;

typedef unsigned int device_type_t;

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

    // Type identifier
    device_type_t type;

    // Device id
    unsigned int id;

    // Pointer to the bus the device is on
    struct bus *bus;

    // Pointer to file_operations structure for the device
    struct file_operations file_operations;
};

struct device_iterator {
    struct device **device_list;
    int *device_list_len;
    int current_idx;
};

/**
 * @brief Initialize the device subsystem
 *
 */
void
device_init();

/**
 * @brief Get the next device id number
 *
 * @return int  Device Id
 */
int
device_get_next_device_id();

/**
 * @brief Add a device to the system
 *
 * @param device    Pointer to the device
 * @return int      Status code
 */
int
device_add_device(struct device *device);

/**
 * @brief Find a device by name
 *
 * @param name              Name of the device
 * @return struct device*   Pointer to the device if found or NULL
 */
struct device *
device_find(const char *name);

int
device_iterator_init(struct device_iterator **iter);

void
device_iterator_free(struct device_iterator *iter);

struct device *
device_iterator_val(struct device_iterator *iter);

int
device_iterator_next(struct device_iterator *iter);

#endif