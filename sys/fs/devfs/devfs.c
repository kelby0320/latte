#include "fs/devfs/devfs.h"

#include "bus/mass_storage.h"
#include "config.h"
#include "dev/block/block.h"
#include "dev/device.h"
#include "errno.h"
#include "fs/fs.h"
#include "fs/path.h"
#include "kernel.h"
#include "libk/kheap.h"
#include "libk/print.h"
#include "libk/string.h"
#include "vfs/file_descriptor.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEVFS_DEVICE_LIST_LENGTH 32

/**
 * @brief Private devfs filesystem data
 *
 */
struct devfs_private {
    // List of block devices
    struct device *block_devices[DEVFS_DEVICE_LIST_LENGTH];

    // List of other device not of a known type
    struct device *raw_devices[DEVFS_DEVICE_LIST_LENGTH];

    // Length of block device list
    size_t block_devices_len;

    // Length of raw device list
    size_t raw_devices_len;
};

/**
 * @brief Private devfs file descriptor data
 *
 */
struct devfs_descriptor_private {
    // Pointer to device
    struct device *device;

    // Read/Write offset
    size_t offset;
};

static bool
is_devfs_block_device(struct block_device *block_device)
{
    struct mass_storage_bus *bus = (struct mass_storage_bus *)block_device->device.bus;
    char bus_name[LATTE_BUS_NAME_MAX_SIZE];

    int num_read = bus->read(bus, 0, 0, bus_name, LATTE_BUS_NAME_MAX_SIZE);
    if (num_read <= 0) {
        return false;
    }

    if (strncmp(bus_name, "devfs", 5) == 0) {
        return true;
    }

    return false;
}

/**
 * @brief Add a device to devfs
 *
 * @param devfs_private     Pointer to private devfs data
 * @param device            Pointer to the device
 * @return int              Status code
 */
static int
devfs_private_add_device(struct devfs_private *devfs_private, struct device *device)
{
    switch (device->type) {
    case DEVICE_TYPE_RAW:
        devfs_private->raw_devices[devfs_private->raw_devices_len] = device;
        devfs_private->raw_devices_len++;
        return 0;
    case DEVICE_TYPE_BLOCK:
        devfs_private->block_devices[devfs_private->block_devices_len] = device;
        devfs_private->block_devices_len++;
        return 0;
    default:
        return -EINVAL;
    }
}

/**
 * @brief Initialize devfs private data
 *
 * @param devfs_private Pointer to devfs private data
 * @return int          Status code
 */
static int
devfs_private_init(struct devfs_private *devfs_private)
{
    struct device_iterator *device_iter;
    int res = device_iterator_init(&device_iter);
    if (res < 0) {
        return res;
    }

    while (true) {
        struct device *device = device_iterator_val(device_iter);
        if (!device) {
            break;
        }

        res = devfs_private_add_device(devfs_private, device);
        if (res < 0) {
            printk("Unknown device with type %d\n", device->type);
        }

        device_iterator_next(device_iter);
    }

    device_iterator_free(device_iter);

    return 0;
}

/**
 * @brief Match a device in a device list
 *
 * @param device_list       Device list to match against
 * @param device_list_len   Device list length
 * @param path_element      Device path
 * @return struct device*   Pointer to matched device or NULL
 */
static struct device *
match_device_in_list(struct device **device_list, size_t device_list_len,
                     struct path_element *path_element)
{
    for (size_t i = 0; i < device_list_len; i++) {
        struct device *device = device_list[i];
        if (strcmp(device->name, path_element->element) == 0) {
            return device;
        }
    }

    return NULL;
}

/**
 * @brief Match a device path to a device
 *
 * @param devfs_private     Pointer to devfs private data
 * @param path              Device path
 * @return struct device*   Matched device or NULL
 */
static struct device *
match_device(struct devfs_private *devfs_private, struct path *path)
{
    if (strcmp(path->root->element, "block") == 0) {
        return match_device_in_list(devfs_private->block_devices, devfs_private->block_devices_len,
                                    path->root->next);
    }

    return match_device_in_list(devfs_private->raw_devices, devfs_private->raw_devices_len,
                                path->root);
}

/**
 * @brief Open a file on a devfs filesystem
 *
 * @param fs_private    Pointer to private filesystem data
 * @param path          Pointer to the path
 * @param mode          File access mode
 * @param out           Pointer to output private descriptor data
 * @return int          Status code
 */
static int
devfs_open(void *fs_private, struct path *path, file_mode_t mode, void **out)
{
    struct devfs_private *devfs_private = (struct devfs_private *)fs_private;

    struct device *device = match_device(devfs_private, path);
    if (!device) {
        return -EEXIST;
    }

    struct devfs_descriptor_private *descriptor_private =
        kzalloc(sizeof(struct devfs_descriptor_private));
    if (!descriptor_private) {
        return -ENOMEM;
    }

    descriptor_private->device = device;
    descriptor_private->offset = 0;

    *out = descriptor_private;

    return 0;
}

/**
 * @brief
 *
 * @param fs_private
 * @return int
 */
static int
devfs_close(void *fs_private)
{
    // TODO
    return 0;
}

/**
 * @brief Read data from a devfs open file
 *
 * @param file_descriptor   Pointer to the file descriptor
 * @param buf               Pointer to the output buffer
 * @param count             Number of bytes to read
 * @return int              Number of bytes actually read
 */
static int
devfs_read(struct file_descriptor *file_descriptor, char *buf, size_t count)
{
    struct devfs_descriptor_private *descriptor_private = file_descriptor->private;
    struct device *device = descriptor_private->device;
    size_t offset = descriptor_private->offset;

    if (device->file_operations.read != NULL) {
        return device->file_operations.read(device, offset, buf, count);
    }

    return -EIO;
}

/**
 * @brief Write data to a devfs open file
 *
 * @param file_descriptor   Pointer to the file descriptor
 * @param buf               Pointer to the input buffer
 * @param count             Number of bytes to write
 * @return int              Number of bytes actually written
 */
static int
devfs_write(struct file_descriptor *file_descriptor, const char *buf, size_t count)
{
    struct devfs_descriptor_private *descriptor_private = file_descriptor->private;
    struct device *device = descriptor_private->device;
    size_t offset = descriptor_private->offset;

    if (device->file_operations.write != NULL) {
        return device->file_operations.write(device, offset, buf, count);
    }

    return -EIO;
}

/**
 * @brief
 *
 * @param file_descriptor
 * @param stat
 * @return int
 */
static int
devfs_stat(struct file_descriptor *file_descriptor, struct file_stat *stat)
{
}

/**
 * @brief
 *
 * @param file_descriptor
 * @param offset
 * @param seek_mode
 * @return int
 */
static int
devfs_seek(struct file_descriptor *file_descriptor, size_t offset, file_seek_mode_t seek_mode)
{
    struct devfs_descriptor_private *descriptor_private = file_descriptor->private;
    struct device *device = descriptor_private->device;

    descriptor_private->offset = offset;

    if (device->file_operations.seek != NULL) {
        return device->file_operations.seek(device, offset, seek_mode);
    }

    return -EIO;
}

/**
 * @brief Resolve a devfs filesystem
 *
 * @param block_device  Pointer to a block device
 * @return void*        Pointer to private filesystem data
 */
static void *
devfs_resolve(struct block_device *block_device)
{
    if (!is_devfs_block_device(block_device)) {
        return NULL;
    }

    struct devfs_private *fs_private = kzalloc(sizeof(struct devfs_private));
    if (!fs_private) {
        return NULL;
    }

    int res = devfs_private_init(fs_private);
    if (res < 0) {
        kfree(fs_private);
        return NULL;
    }

    return fs_private;
}

struct filesystem *
devfs_init()
{
    struct filesystem *fs = kzalloc(sizeof(struct filesystem));
    if (!fs) {
        panic("Unable to initialize dev filesystem");
    }

    fs->open = devfs_open;
    fs->close = devfs_close;
    fs->read = devfs_read;
    fs->write = devfs_write;
    fs->stat = devfs_stat;
    fs->seek = devfs_seek;
    fs->resolve = devfs_resolve;

    strncpy(fs->name, "devfs", FILESYSTEM_NAME_MAX_LEN);

    return fs;
}