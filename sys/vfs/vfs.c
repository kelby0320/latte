#include "vfs/vfs.h"

#include "config.h"
#include "dev/block/block.h"
#include "dev/device.h"
#include "errno.h"
#include "fs/fs.h"
#include "fs/path.h"
#include "kernel.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "libk/string.h"
#include "vfs/file_descriptor.h"
#include "vfs/mountpoint.h"

/**
 * @brief Determine whether a device is a boot device
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
 * @brief Find a bootable block device
 *
 * @param block_device_out  Output pointer to boot block device
 * @return int              Status code
 */
static int
vfs_find_boot_block_device(struct block_device **block_device_out)
{
    struct device_iterator *iter;
    int res = device_iterator_init(&iter);
    if (res < 0) {
        return res;
    }

    while (true) {
        struct device *device = device_iterator_val(iter);
        if (!device) {
            break;
        }

        if (is_boot_device(device)) {
            *block_device_out = (struct block_device *)device;
            return 0;
        }

        device_iterator_next(iter);
    }

    device_iterator_free(iter);

    return -EEXIST;
}

int
vfs_init()
{
    file_descriptor_init();
    mountpoint_init();

    struct block_device *boot_block_device;
    int res = vfs_find_boot_block_device(&boot_block_device);
    if (res < 0) {
        goto err_out;
    }

    res = vfs_mount("/", boot_block_device->device.name);
    if (res < 0) {
        goto err_out;
    }

    return 0;

err_out:
    panic("Unable to mount root filesystem");
}

int
vfs_mount(const char *path, const char *blk_dev_name)
{
    struct device *device = device_find(blk_dev_name);
    if (!device) {
        return -ENOENT;
    }

    struct block_device *block_device = (struct block_device *)device;

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

    res = mountpoint_add(mountpoint);
    if (res < 0) {
        goto err_out;
    }

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
    // TODO
    return -1;
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
