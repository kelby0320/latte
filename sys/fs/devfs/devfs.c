#include "fs/devfs/devfs.h"

#include "config.h"
#include "dev/block/block.h"
#include "dev/device.h"
#include "errno.h"
#include "fs/fs.h"
#include "fs/path.h"
#include "kernel.h"
#include "libk/alloc.h"
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
    // List of devices
    struct device *devices[DEVFS_DEVICE_LIST_LENGTH];

    // Length of block device list
    size_t devices_len;
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

#define as_devfs_private(ptr)            ((struct devfs_private *)ptr)
#define as_devfs_descriptor_private(ptr) ((struct devfs_descriptor_private *)ptr)

static bool
is_devfs_block_device(struct block_device *block_device)
{
    char buf[LATTE_SECTOR_SIZE];

    int num_read = block_device_read_sectors(block_device, 0, buf, 1);
    if (num_read < 0) {
        return false;
    }

    if (strncmp(buf, "devfs", 5) == 0) {
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

    devfs_private->devices[devfs_private->devices_len] = device;
    devfs_private->devices_len++;

    return 0;
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
    for_each_device(device)
    {
        int res = devfs_private_add_device(devfs_private, device);
        if (res < 0) {
            printk("Unknown device with type %d\n", device->type);
        }
    }
    for_each_device_end();

    return 0;
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
    struct device **device_list = devfs_private->devices;
    size_t device_list_len = devfs_private->devices_len;
    char *match_name = path->root->next->element; // Second elemnt in the path, e.g. /dev/device0

    for (size_t i = 0; i < device_list_len; i++) {
        struct device *device = device_list[i];
        if (strcmp(device->name, match_name) == 0) {
            return device;
        }
    }

    return NULL;
}

static bool
is_valid_path(struct path *path)
{
    // First element must be dev
    if (strcmp(path->root->element, "dev") != 0) {
        return false;
    }

    // There must be a second element
    if (path->root->next == NULL) {
        return false;
    }

    // There must NOT be a third element
    if (path->root->next->next != NULL) {
        return false;
    }

    return true;
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
    struct devfs_private *devfs_private = as_devfs_private(fs_private);

    if (!is_valid_path(path)) {
        return -EINVAL;
    }

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