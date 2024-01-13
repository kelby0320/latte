#include "vfs/vfs.h"

#include "config.h"
#include "dev/block/block.h"
#include "dev/device.h"
#include "errno.h"
#include "fs/fs.h"
#include "fs/path.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "libk/string.h"
#include "vfs/file_descriptor.h"
#include "vfs/mountpoint.h"

int
vfs_init()
{
    file_descriptor_init();
    mountpoint_init();

    // TODO - find and mount root filesystem
    return 0;
}

int
vfs_mount(const char *path, const char *blk_dev_name, unsigned int part_no)
{
    struct device *device = device_find(blk_dev_name);
    if (!device) {
        return -ENOENT;
    }

    struct block_device *block_device = (struct block_device *)device;
    struct partition_table_entry *part_entry = &block_device->partition_table[part_no];

    struct mountpoint *mountpoint = kzalloc(sizeof(struct mountpoint));
    if (!mountpoint) {
        return -ENOMEM;
    }

    int res = partition_init(&mountpoint->partition, block_device, part_entry);
    if (res < 0) {
        goto err_out;
    }

    res = partition_bind_fs(&mountpoint->partition);
    if (res < 0) {
        goto err_out;
    }

    strncpy(mountpoint->path, path, LATTE_MAX_PATH_LEN);

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

    struct partition *partition = &mountpoint->partition;
    struct filesystem *filesystem = partition->filesystem;

    res = filesystem->open(partition, path, mode, descriptor->private);
    if (res < 0) {
        goto err_out2;
    }

    descriptor->partition = partition;

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

    struct partition *partition = descriptor->partition;
    struct filesystem *filesystem = partition->filesystem;

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

    struct partition *partition = descriptor->partition;
    struct filesystem *filesystem = partition->filesystem;

    return filesystem->stat(descriptor, stat);
}

int
vfs_seek(int fd, int offset, file_seek_mode_t seek_mode)
{
    struct file_descriptor *descriptor = file_descriptor_get(fd);
    if (!descriptor) {
        return -EINVAL;
    }

    struct partition *partition = descriptor->partition;
    struct filesystem *filesystem = partition->filesystem;

    return filesystem->seek(descriptor, offset, seek_mode);
}
