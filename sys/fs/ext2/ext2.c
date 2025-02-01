#include "fs/ext2.h"

#include "buffered_reader.h"
#include "config.h"
#include "errno.h"
#include "file_descriptor.h"
#include "fs.h"
#include "fs/ext2/common.h"
#include "fs/ext2/dir.h"
#include "fs/ext2/dir_iter.h"
#include "fs/ext2/inode.h"
#include "kernel.h"
#include "libk/alloc.h"
#include "libk/memory.h"
#include "libk/string.h"
#include "mountpoint.h"
#include "path.h"

#include <stdint.h>

/**
 * @brief Read the superblock of the filesystem
 *
 * @param fs_private    Pointer to private fs data
 * @return int          Number of bytes read
 */
static int
ext2_read_superblock(struct ext2_private *fs_private)
{
    struct block_buffered_reader *reader = fs_private->reader;
    block_buffered_reader_seek(reader, EXT2_SUPERBLOCK_OFFSET);
    int res = block_buffered_reader_read(
        reader, (char *)&fs_private->superblock, EXT2_SUPERBLOCK_SIZE);
    return res;
}

/**
 * @brief Find the inode corresponding to a file path
 *
 * @param path          Pointer to the path
 * @param fs_private    Pointer to private fs data
 * @return struct inode* Inode structure or NULL
 */
static struct ext2_inode *
ext2_path_to_inode(struct path *path, struct ext2_private *fs_private)
{
    struct path_element *path_element = path->root;
    struct ext2_inode *inode, *dir_inode;

    // Read root directory
    int res = ext2_read_inode(&dir_inode, fs_private, EXT2_ROOT_DIR_INODE);
    if (res < 0) {
        return NULL;
    }

    // If path_element is any empty string return root directory inode
    if (path_element->element[0] == 0) {
        return dir_inode;
    }

    while (1) {
        // Read inode from directory
        res = ext2_dir_find_entry(
            &inode, fs_private, dir_inode, path_element->element);
        if (res < 0) {
            goto err_out;
        }

        if (!path_element->next) {
            // No more path elements, we have final inode
            return inode;
        }

        if (!(inode->i_mode & EXT2_S_IFDIR)) {
            // Not a directory, Error!
            goto err_out;
        }

        // Search next directory
        kfree(dir_inode);
        dir_inode = inode;
        path_element = path_element->next;
    }

err_out:
    if (dir_inode) {
        kfree(dir_inode);
    }

    if (inode) {
        kfree(inode);
    }

    return NULL;
}

/**
 * @brief Open a file on an Ext2 filesystem
 *
 * @param fs_private    Pointer to the filesystem private data
 * @param path          Pointer to filepath
 * @param mode          Open file mode
 * @param out           Pointer to output private descriptor data
 * @return int          Status code
 */
static int
ext2_open(void *fs_private, struct path *path, file_mode_t mode, void **out)
{
    if (mode != FILE_MODE_READ) {
        return -EACCES;
    }

    struct ext2_descriptor_private *descriptor_private =
        kzalloc(sizeof(struct ext2_descriptor_private));
    if (!descriptor_private) {
        return -ENOMEM;
    }

    struct ext2_inode *inode =
        ext2_path_to_inode(path, as_ext2_private(fs_private));
    if (!inode) {
        kfree(descriptor_private);
        return -EEXIST;
    }

    if (!(inode->i_mode & EXT2_S_IFREG)) {
        kfree(descriptor_private);
        return -EACCES;
    }

    descriptor_private->inode = inode;
    descriptor_private->blk_offset = 0;
    descriptor_private->byte_offset = 0;

    *out = descriptor_private;

    return 0;
}

/**
 * @brief Close an Ext2 open file
 *
 * @param file_descriptor    Pointer to the file descriptor
 * @return int               Status code
 */
static int
ext2_close(struct file_descriptor *file_descriptor)
{
    return -1;
}

/**
 * @brief Read data from an Ext2 open file
 *
 * @param file_descriptor   Pointer to the file descriptor
 * @param buf               Pointer to the output buffer
 * @param count             Number of bytes to read
 * @return int              Number of bytes actually read
 */
static int
ext2_read(struct file_descriptor *file_descriptor, char *buf, size_t count)
{
    struct ext2_descriptor_private *descriptor_private =
        file_descriptor->private;
    struct ext2_private *fs_private = file_descriptor->mountpoint->fs_private;

    struct ext2_inode *inode = descriptor_private->inode;
    int blk_offset = descriptor_private->blk_offset;
    int byte_offset = descriptor_private->byte_offset;

    return ext2_read_inode_data(
        fs_private, inode, buf, count, blk_offset, byte_offset);
}

/**
 * @brief Write data to an Ext2 open file
 *
 * @param file_descriptor   Pointer to the file descriptor
 * @param buf               Pointer to the input buffer
 * @param count             Number of bytes to write
 * @return int              Number of bytes actually written
 */
static int
ext2_write(
    struct file_descriptor *file_descriptor, const char *buf, size_t count)
{
    return 0;
}

/**
 * @brief Read the status of an Ext2 open file
 *
 * @param file_descriptor   Pointer to the file descriptor
 * @param stat  Pointer to output stat structure
 * @return int  Status code
 */
static int
ext2_stat(struct file_descriptor *file_descriptor, struct file_stat *stat)
{
    struct ext2_descriptor_private *descriptor_private =
        file_descriptor->private;

    stat->size = descriptor_private->inode->i_size;
    stat->mode = descriptor_private->inode->i_mode;

    return 0;
}

/**
 * @brief Seek to a location in an Ext2 open file
 *
 * @param file_descriptor   Pointer to the file descriptor
 * @param offset            Seek offset
 * @param seek_mode         Seek mode
 * @return int              Status code
 */
static int
ext2_seek(
    struct file_descriptor *file_descriptor, uint32_t offset,
    file_seek_mode_t seek_mode)
{
    struct ext2_descriptor_private *descriptor_private =
        file_descriptor->private;
    struct ext2_private *fs_private = file_descriptor->mountpoint->fs_private;
    uint32_t block_size = fs_private->block_size;

    switch (seek_mode) {
    case SEEK_SET:
        descriptor_private->blk_offset = offset / block_size;
        descriptor_private->byte_offset = offset % block_size;
        break;
    case SEEK_CUR:
        descriptor_private->blk_offset =
            descriptor_private->blk_offset + offset / block_size;
        descriptor_private->byte_offset = offset % block_size;
        break;
    case SEEK_END:
        descriptor_private->blk_offset =
            descriptor_private->inode->i_size + offset / block_size;
        descriptor_private->byte_offset = offset % block_size;
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

/**
 * @brief Attempt to bind an Ext2 filesystem to the disk
 *
 * @param block        Pointer to the block device
 * @return void *      Pointer to private Ext2 data structure
 */
static void *
ext2_resolve(struct block *block)
{
    struct ext2_private *fs_private = kzalloc(sizeof(struct ext2_private));
    if (!fs_private) {
        return NULL;
    }

    struct block_buffered_reader *reader =
        kzalloc(sizeof(struct block_buffered_reader));
    if (!reader) {
        goto err_reader_alloc;
    }

    block_buffered_reader_init(reader, block);
    fs_private->reader = reader;

    int res = ext2_read_superblock(fs_private);
    if (res < 0) {
        goto err_superblock;
    }

    if (fs_private->superblock.s_magic != EXT2_MAGIC_NUMBER) {
        goto err_superblock;
    }

    fs_private->block_size = 1024 << fs_private->superblock.s_log_block_size;

    return fs_private;

err_superblock:
    kfree(reader);

err_reader_alloc:
    kfree(fs_private);
    return NULL;
}

static int
ext2_opendir(void *fs_private, struct path *path, void **out)
{
    struct ext2_descriptor_private *descriptor_private =
        kzalloc(sizeof(struct ext2_descriptor_private));
    if (!descriptor_private) {
        return -ENOMEM;
    }

    struct ext2_inode *inode =
        ext2_path_to_inode(path, as_ext2_private(fs_private));
    if (!inode) {
        kfree(descriptor_private);
        return -EEXIST;
    }

    if (!(inode->i_mode & EXT2_S_IFDIR)) {
        kfree(descriptor_private);
        return -EACCES;
    }

    struct ext2_dir_iter *dir_iter = kzalloc(sizeof(struct ext2_dir_iter));
    if (!dir_iter) {
        return -ENOMEM;
    }

    int res = ext2_dir_iter_init(dir_iter, fs_private, inode);
    if (res < 0) {
        goto err_iter_init;
    }

    descriptor_private->inode = inode;
    descriptor_private->dir_iter = dir_iter;
    descriptor_private->blk_offset = 0;
    descriptor_private->byte_offset = 0;

    *out = descriptor_private;

    return 0;

err_iter_init:
    kfree(dir_iter);
    return res;
}

static int
ext2_closedir(struct file_descriptor *file_descriptor)
{
    return -1;
}

static int
ext2_readdir(
    struct file_descriptor *file_descriptor, struct dir_entry *dir_entry_out)
{
    if (file_descriptor->type != FT_DIRECTORY) {
        return -EINVAL;
    }

    struct ext2_descriptor_private *descriptor_private =
        file_descriptor->private;
    struct ext2_dir_iter *dir_iter = descriptor_private->dir_iter;
    struct ext2_private *fs_private = file_descriptor->mountpoint->fs_private;

    return ext2_dir_next_entry(
        fs_private, descriptor_private->inode, dir_iter, dir_entry_out);
}

int
ext2_mkdir(void *fs_private, struct path *path)
{
    return -1;
}

struct filesystem *
ext2_init()
{
    struct filesystem *fs = kzalloc(sizeof(struct filesystem));
    if (!fs) {
        printk("ext2 *fs %d\n", (unsigned int)fs);
        panic("Unable to initialize ext2 filesystem");
    }

    fs->open = ext2_open;
    fs->close = ext2_close;
    fs->read = ext2_read;
    fs->write = ext2_write;
    fs->stat = ext2_stat;
    fs->seek = ext2_seek;
    fs->resolve = ext2_resolve;
    fs->opendir = ext2_opendir;
    fs->closedir = ext2_closedir;
    fs->readdir = ext2_readdir;
    fs->mkdir = ext2_mkdir;

    strncpy(fs->name, "ext2", FILESYSTEM_NAME_MAX_LEN);

    return fs;
}
