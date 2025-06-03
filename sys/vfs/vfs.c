#include "vfs.h"

#include "block.h"
#include "bus.h"
#include "config.h"
#include "device.h"
#include "errno.h"
#include "file_descriptor.h"
#include "fs.h"
#include "kernel.h"
#include "libk/alloc.h"
#include "libk/memory.h"
#include "libk/print.h"
#include "libk/string.h"
#include "mountpoint.h"
#include "path.h"

#include <stdbool.h>

/**
 * @brief Predicate to determine whether a block is a bootable
 *
 * @param block    Pointer to the block
 * @return true
 * @return false
 */
static inline bool
is_boot_block(struct block *block)
{
    return block->bootable;
}

/**
 * @brief Predicate to determine whether a device is a virutal block device
 *
 * @param device    Pointer to a device
 * @return true
 * @return false
 */
static bool
is_devfs_block(struct block *block)
{
    if (strncmp(block->name, "devfs", 5) == 0) {
        return true;
    }

    return false;
}

/**
 * @brief Find a block device and mount it
 *
 * @param mount_path    Path to mount device
 * @param predicate     Predicate function to identify the device
 * @return int          Status code
 */
static int
vfs_find_and_mount(const char *mount_path, bool (*predicate)(struct block *))
{
    struct block *block = block_find(predicate);
    if (!block) {
        return -ENOENT;
    }

    int res = vfs_mount(mount_path, block);
    if (res < 0) {
        return res;
    }

    return 0;
}

int
vfs_init()
{
    int res = vfs_find_and_mount("/", is_boot_block);
    if (res < 0) {
        panic("Unable to mount root filesystem\n");
    }

    res = vfs_find_and_mount("/dev", is_devfs_block);
    if (res < 0) {
        panic("Unable to mount devfs filesystem\n");
    }

    return 0;
}

int
vfs_mount(const char *path, struct block *block)
{
    struct mountpoint *mountpoint = kzalloc(sizeof(struct mountpoint));
    if (!mountpoint) {
        return -ENOMEM;
    }

    strncpy(mountpoint->path, path, LATTE_MAX_PATH_LEN);
    mountpoint->block = block;

    int res = fs_resolve(mountpoint);
    if (res < 0) {
        goto err_out;
    }

    printk(
        "Found %s filesystem on %s\n", mountpoint->filesystem->name,
        mountpoint->block->name);

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
        goto err_path;
    }

    struct mountpoint *mountpoint = mountpoint_find(filename);
    if (!mountpoint) {
        res = -ENOENT;
        goto err_mount;
    }

    file_mode_t mode = fs_get_mode_from_string(mode_str);
    if (mode == FILE_MODE_INVALID) {
        res = -EINVAL;
        goto err_mode;
    }

    struct filesystem *filesystem = mountpoint->filesystem;
    void *fs_private = mountpoint->fs_private;

    res = filesystem->open(fs_private, path, mode, &descriptor->private);
    if (res < 0) {
        goto err_open;
    }

    descriptor->type = FT_REGULAR_FILE;
    descriptor->mountpoint = mountpoint;

    path_free(path);

    return descriptor->index;

err_open:
err_mount:
err_mode:
    path_free(path);

err_path:
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

    if (descriptor->type != FT_REGULAR_FILE) {
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

int
vfs_opendir(const char *dirname)
{
    struct file_descriptor *descriptor;
    int res = file_descriptor_get_new(&descriptor);
    if (res < 0) {
        return res;
    }

    struct path *path;
    path_from_str(&path, dirname);
    if (!path) {
        res = -EACCES;
        goto err_path;
    }

    struct mountpoint *mountpoint = mountpoint_find(dirname);
    if (!mountpoint) {
        res = -ENOENT;
        goto err_mount;
    }

    struct filesystem *filesystem = mountpoint->filesystem;
    void *fs_private = mountpoint->fs_private;

    res = filesystem->opendir(fs_private, path, &descriptor->private);
    if (res < 0) {
        goto err_opendir;
    }

    descriptor->type = FT_DIRECTORY;
    descriptor->mountpoint = mountpoint;

    path_free(path);

    return descriptor->index;

err_opendir:
err_mount:
    path_free(path);

err_path:
    file_descriptor_free(descriptor);

    return res;
}

int
vfs_closedir(int fd)
{
    return -1;
}

int
vfs_readdir(int fd, struct dir_entry *entry)
{
    struct file_descriptor *descriptor = file_descriptor_get(fd);
    if (!descriptor) {
        return -EINVAL;
    }

    if (descriptor->type != FT_DIRECTORY) {
        return -EINVAL;
    }

    struct filesystem *filesystem = descriptor->mountpoint->filesystem;

    return filesystem->readdir(descriptor, entry);
}

int
vfs_mkdir(const char *path)
{
    return -1;
}
