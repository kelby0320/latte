#include "fs/devfs/devfs.h"

#include "block/block.h"
#include "block/buffered_reader.h"
#include "config.h"
#include "dev/device.h"
#include "drivers/driver.h"
#include "errno.h"
#include "fs/fs.h"
#include "fs/path.h"
#include "kernel.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/print.h"
#include "libk/string.h"
#include "vfs/file_descriptor.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Private devfs filesystem data
 *
 */
struct devfs_private {
    // Pointer to buffered reader for the filesystem
    struct block_buffered_reader *reader;
};

struct devfs_node {
    struct device *device;
    struct file_operations *fops;
};

/**
 * @brief Private devfs file descriptor data
 *
 */
struct devfs_descriptor_private {
    // Pointer to device
    struct devfs_node *devfs_node;

    // Read/Write offset
    size_t offset;
};

#define as_devfs_private(ptr)            ((struct devfs_private *)ptr)
#define as_devfs_descriptor_private(ptr) ((struct devfs_descriptor_private *)ptr)

static struct list_item *node_list = NULL;
static size_t num_nodes = 0;

/**
 * @brief Check if block is a devfs block device
 *
 * @param block    Pointer to block
 * @return bool
 */
static bool
is_devfs_block_device(struct block *block)
{
    if (strncmp(block->name, "devfs", 5) != 0) {
	return false;
    }
    
    char buf[LATTE_SECTOR_SIZE];

    struct block_buffered_reader reader;
    block_buffered_reader_init(&reader, block);

    int num_read = block_buffered_reader_read(&reader, buf, LATTE_SECTOR_SIZE);
    if (num_read != LATTE_SECTOR_SIZE) {
        return false;
    }

    /* Devfs should read all 0s */
    for (int i = 0; i < num_read; i++) {
	if (buf[i] != 0) {
	    return false;
	}
    }

    return true;
}

/**
 * @brief Match a path to a devfs_node
 *
 * @param devfs_private           Pointer to devfs private data
 * @param path                    Device path
 * @return struct devfs_node*     Matched devfs_node or NULL
 */
static struct devfs_node *
match_device(struct devfs_private *devfs_private, struct path *path)
{
    char *match_name = path->root->next->element; // Second elemnt in the path, e.g. /dev/device0

    for_each_in_list(struct devfs_node *, node_list, list, node) {
	struct device *device = node->device;
	if (strcmp(device->name, match_name) == 0) {
	    return node;
	}
    }

    return NULL;
}

/**
 * @brief Check if path is a valid devfs path
 *
 * @param path    Pointer to path object
 * @return bool
 */
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

    struct devfs_node *devfs_node = match_device(devfs_private, path);
    if (!devfs_node) {
        return -EEXIST;
    }

    struct devfs_descriptor_private *descriptor_private = kzalloc(sizeof(struct devfs_descriptor_private));
    if (!descriptor_private) {
        return -ENOMEM;
    }

    descriptor_private->devfs_node = devfs_node;
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
    return -EIO;
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
    struct devfs_node *devfs_node = descriptor_private->devfs_node;
    size_t offset = descriptor_private->offset;

    if (devfs_node->fops->read != NULL) {
        return devfs_node->fops->read(devfs_node->device, offset, buf, count);
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
    struct devfs_node *devfs_node = descriptor_private->devfs_node;
    size_t offset = descriptor_private->offset;

    if (devfs_node->fops->write != NULL) {
        return devfs_node->fops->write(devfs_node->device, offset, buf, count);
    }

    return -EIO;
}

/**
 * @brief Read stat info from a devfs open file
 *
 * @param file_descriptor  Pointer to the file descriptor
 * @param stat             Pointer to the stat structure
 * @return int             Status code
 */
static int
devfs_stat(struct file_descriptor *file_descriptor, struct file_stat *stat)
{
    // TODO
    return -EIO;
}

/**
 * @brief Seek on a devfs open file
 *
 * @param file_descriptor  Pointer to the file descriptor
 * @param offset           Seek offset
 * @param seek_mode        Seek mode
 * @return int             New offset location
 */
static int
devfs_seek(struct file_descriptor *file_descriptor, size_t offset, file_seek_mode_t seek_mode)
{
    struct devfs_descriptor_private *descriptor_private = file_descriptor->private;
    struct devfs_node *devfs_node = descriptor_private->devfs_node;

    descriptor_private->offset = offset;

    if (devfs_node->fops->seek != NULL) {
        return devfs_node->fops->seek(devfs_node->device, offset, seek_mode);
    }

    return -EIO;
}

/**
 * @brief Resolve a devfs filesystem
 *
 * @param block  Pointer to a block device
 * @return void*        Pointer to private filesystem data
 */
static void *
devfs_resolve(struct block *block)
{
    if (!is_devfs_block_device(block)) {
        return NULL;
    }

    struct devfs_private *fs_private = kzalloc(sizeof(struct devfs_private));
    if (!fs_private) {
        return NULL;
    }

    struct block_buffered_reader *reader = kzalloc(sizeof(struct block_buffered_reader));
    if (!reader) {
	goto err_reader_alloc;
    }

    block_buffered_reader_init(reader, block);
    fs_private->reader = reader;

    return fs_private;

err_reader_alloc:
    kfree(fs_private);
    return NULL;
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

int
devfs_make_node(struct device *device, struct file_operations *fops)
{
    struct devfs_node *devfs_node = kzalloc(sizeof(struct devfs_node));
    if (!devfs_node) {
	return -ENOMEM;
    }

    devfs_node->device = device;
    devfs_node->fops = fops;

    return list_push_back(&node_list, devfs_node);
}
