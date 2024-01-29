#ifndef DEVICE_H
#define DEVICE_H

#include "config.h"
#include "fs/fs.h"

#include <stddef.h>

#define DEVICE_TYPE_RAW   0
#define DEVICE_TYPE_BLOCK 1
#define DEVICE_TYPE_VGA   2
#define DEVICE_TYPE_TERM  3

struct bus;
struct device;

typedef unsigned int device_type_t;

/**
 * @brief File operations structure for devices
 *
 */
struct file_operations {
    int (*read)(struct device *device, size_t offset, char *buf, size_t count);
    int (*write)(struct device *device, size_t offset, const char *buf, size_t count);
    int (*seek)(struct device *device, size_t offset, file_seek_mode_t seek_mode);
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

    // Pointer to the bus the device is on
    struct bus *bus;

    // Pointer to file_operations structure for the device
    struct file_operations file_operations;
};

/**
 * @brief Device iterator structure
 *
 */
struct device_iterator {
    // Pointer to the device list
    struct device **device_list;

    // Pointer to the device list length
    int *device_list_len;

    // Current index of the device iterator
    int current_idx;
};

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

/**
 * @brief Initialize a device iterator
 *
 * @param iter  Pointer to the device iterator
 * @return int  Status code
 */
int
device_iterator_init(struct device_iterator **iter);

/**
 * @brief Free a device iterator
 *
 * @param iter  Pointer to the iterator
 */
void
device_iterator_free(struct device_iterator *iter);

/**
 * @brief Get the current device associated with a device iterator
 *
 * @param iter              Pointer to the device iterator
 * @return struct device*   Pointer to the associated device
 */
struct device *
device_iterator_val(struct device_iterator *iter);

/**
 * @brief Increment the device iterator
 *
 * @param iter  Pointer to the device iterator
 * @return int  Status code
 */
int
device_iterator_next(struct device_iterator *iter);

#endif