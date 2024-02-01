#include "vfs/vfs.h"

#include "bus/bus.h"
#include "config.h"
#include "dev/block/block.h"
#include "dev/device.h"
#include "errno.h"
#include "fs/fs.h"
#include "fs/path.h"
#include "kernel.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "libk/print.h"
#include "libk/string.h"
#include "vfs/file_descriptor.h"
#include "vfs/mountpoint.h"
#include <stdbool.h>

/**
 * @brief Predicate to determine whether a device is a boot device
 *
 * @param device    Pointer to the device
 * @return true
 * @return false
 */
static bool
is_boot_device(struct device *device)
{
    if (device->type == DEVICE_TYPE_BLOCK) {
        struct block_device *block_device = (struct block_device *)device;
        if (block_device->type == BLOCK_DEVICE_TYPE_PART) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Predicate to determine whether a device is a virutal block device
 *
 * @param device    Pointer to a device
 * @return true
 * @return false
 */
static bool
is_virtual_block_device(struct device *device)
{
    if (device->type == DEVICE_TYPE_BLOCK) {
        struct block_device *block_device = (struct block_device *)device;
        if (block_device->type == BLOCK_DEVICE_TYPE_VIRT) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Find a specific block device using the give predicate function
 *
 * @param predicate         Pointer to predicate function
 * @param block_device_out  Output pointer to block device
 * @return int              Status code
 */
static int
vfs_find_block_device(bool (*predicate)(struct device *), struct block_device **block_device_out)
{
    for_each_device(device)
    {
        if (predicate(device)) {
            *block_device_out = (struct block_device *)device;
            return 0;
        }
    }
    for_each_device_end();

    return -EEXIST;
}

/**
 * @brief Find a block device and mount it
 *
 * @param mount_path    Path to mount device
 * @param predicate     Predicate function to identify the device
 * @return int          Status code
 */
static int
vfs_find_and_mount(const char *mount_path, bool (*predicate)(struct device *))
{
    struct block_device *block_device;
    int res = vfs_find_block_device(predicate, &block_device);
    if (res < 0) {
        return res;
    }

    res = vfs_mount(mount_path, block_device);
    if (res < 0) {
        return res;
    }

    return 0;
}

int
vfs_init()
{
    int res = vfs_find_and_mount("/", is_boot_device);
    if (res < 0) {
        panic("Unable to mount root filesystem\n");
    }

    res = vfs_find_and_mount("/dev", is_virtual_block_device);
    if (res < 0) {
        panic("Unable to mount devfs filesystem\n");
    }

    return 0;
}

int
vfs_mount(const char *path, struct block_device *block_device)
{
    struct mountpoint *mountpoint = kzalloc(sizeof(struct mountpoint));
    if (!mountpoint) {
        return -ENOMEM;
    }

    strncpy(mountpoint->path, path, LATTE_MAX_PATH_LEN);
    mountpoint->block_device = block_device;

    int res = fs_resolve(mountpoint);
    if (res < 0) {
        goto err_out;
    }

    printk("Found %s filesystem on %s\n", mountpoint->filesystem->name,
           mountpoint->block_device->device.name);

    res = mountpoint_add(mountpoint);
    if (res < 0) {
        goto err_out;
    }

    printk("Mounted %s filesystem on %s\n", mountpoint->filesystem->name, path);

    return 0;

err_out:
    kfree(mountpoint);
    return res;
}

int
vfs_open(const char *filename, const char *mode_str)
{
    struct file_descriptor *descriptor;
    int res = file_descriptor_get_new(&descriptor);
    if (res < 0) {
        return res;
    }

    struct path *path;
    path_from_str(&path, filename);
    if (!path) {
        res = -EACCES;
        goto err_out1;
    }

    struct mountpoint *mountpoint = mountpoint_find(filename);
    if (!mountpoint) {
        res = -ENOENT;
        goto err_out2;
    }

    file_mode_t mode = fs_get_mode_from_string(mode_str);
    if (mode == FILE_MODE_INVALID) {
        res = -EINVAL;
        goto err_out2;
    }

    struct filesystem *filesystem = mountpoint->filesystem;
    void *fs_private = mountpoint->fs_private;

    res = filesystem->open(fs_private, path, mode, &descriptor->private);
    if (res < 0) {
        goto err_out2;
    }

    descriptor->mountpoint = mountpoint;

    path_free(path);

    return descriptor->index;

err_out2:
    path_free(path);

err_out1:
    file_descriptor_free(descriptor);

    return res;
}

int
vfs_close(int fd)
{
    // TODO
    return -1;
}

int
vfs_read(int fd, char *ptr, size_t count)
{
    struct file_descriptor *descriptor = file_descriptor_get(fd);
    if (!descriptor) {
        return -EINVAL;
    }

    struct filesystem *filesystem = descriptor->mountpoint->filesystem;

    return filesystem->read(descriptor, ptr, count);
}

int
vfs_write(int fd, const char *ptr, size_t count)
{
    struct file_descriptor *descriptor = file_descriptor_get(fd);
    if (!descriptor) {
        return -EINVAL;
    }

    struct filesystem *filesystem = descriptor->mountpoint->filesystem;

    return filesystem->write(descriptor, ptr, count);
}

int
vfs_stat(int fd, struct file_stat *stat)
{
    struct file_descriptor *descriptor = file_descriptor_get(fd);
    if (!descriptor) {
        return -EINVAL;
    }

    struct filesystem *filesystem = descriptor->mountpoint->filesystem;

    return filesystem->stat(descriptor, stat);
}

int
vfs_seek(int fd, int offset, file_seek_mode_t seek_mode)
{
    struct file_descriptor *descriptor = file_descriptor_get(fd);
    if (!descriptor) {
        return -EINVAL;
    }

    struct filesystem *filesystem = descriptor->mountpoint->filesystem;

    return filesystem->seek(descriptor, offset, seek_mode);
}
