#include "fs/ext2/ext2.h"

#include "config.h"
#include "dev/disk/disk.h"
#include "dev/disk/buffer/bufferedreader.h"
#include "errno.h"
#include "fs/ext2/common.h"
#include "fs/ext2/dir.h"
#include "fs/ext2/inode.h"
#include "fs/path.h"
#include "kernel.h"
#include "libk/kheap.h"
#include "libk/memory.h"

#include <stdint.h>

static int
ext2_read_superblock(struct ext2_private *fs_private)
{
    bufferedreader_seek(fs_private->reader, EXT2_FS_START + EXT2_SUPERBLOCK_OFFSET);
    int res = bufferedreader_read(fs_private->reader, &fs_private->superblock, EXT2_SUPERBLOCK_SIZE);
    return res;
}



static struct inode*
ext2_path_to_inode(struct path* path, struct disk *disk, struct ext2_private *fs_private)
{
    struct path_element *path_element = path->root->element;
    struct inode *inode, *dir_inode;

    // Read root directory
    int res = ext2_read_inode(&dir_inode, disk, fs_private, EXT2_ROOT_DIR_INODE);
    if (res < 0) {
        return 0;
    }

    while (1) {
        // Read inode from directory
        res = ext2_get_directory_entry(&inode, disk, fs_private, dir_inode, path_element->element);
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
        path_element = path_element->next->element;
    }

err_out:
    if (dir_inode) {
        kfree(dir_inode);
    }

    if (inode) {
        kfree(inode);
    }
    return 0;
}

int
ext2_resolve(struct disk *disk)
{
    struct ext2_private *fs_private = kzalloc(sizeof(struct ext2_private));
    if (!fs_private) {
        return -ENOMEM;
    }

    struct bufferedreader *reader;
    bufferedreader_new(&reader, disk);
    fs_private->reader = reader;

    int res = ext2_read_superblock(fs_private);
    if (res < 0) {
        kfree(fs_private);
        return -EIO;
    }

    if (fs_private->superblock.s_magic != EXT2_MAGIC_NUMBER) {
        kfree(fs_private);
        return -EIO;
    }

    fs_private->block_size = 1024 << fs_private->superblock.s_log_block_size;
    disk->fs_private = fs_private;

    return 0;
}

int
ext2_open(struct disk *disk, struct path *path, FILE_MODE mode, void **out)
{
    if (mode != FILE_MODE_READ) {
        return -EACCES;
    }

    struct ext2_file_descriptor *descriptor  = kzalloc(sizeof(struct ext2_file_descriptor));
    if (!descriptor) {
        return -ENOMEM;
    }

    struct inode *inode = ext2_path_to_inode(path, disk, disk->fs_private);
    if (!inode) {
        kfree(descriptor);
        return -EEXIST;
    }

    if (!(inode->i_mode & EXT2_S_IFREG)) {
        kfree(descriptor);
        return -EACCES;
    }

    descriptor->inode = inode;
    descriptor->offset = 0;

    *out = (void*)descriptor;
    return 0;
}

int
ext2_close(void *private)
{
    return 0;
}

int
ext2_read(struct disk *disk, void *desc, char *buf, uint32_t count)
{
    struct ext2_file_descriptor *descriptor = desc;
    struct ext2_private *fs_private = disk->fs_private;

    int read = ext2_read_inode_data(fs_private, descriptor->inode, buf, count, descriptor->offset);
    return read;
}

int
ext2_write(struct disk *disk, void *desc, const char *buf, uint32_t count)
{
    return 0;
}

int
ext2_seek(void *desc, uint32_t offset, FILE_SEEK_MODE seek_mode)
{
    return 0;
}

int
ext2_stat(struct disk *disk, void *desc, struct file_stat *stat)
{
    return 0;
}

struct filesystem*
ext2_init()
{
    struct filesystem *fs = kzalloc(sizeof(struct filesystem));
    if (!fs) {
        panic("Unable to initialize ext2 filesystem");
    }

    fs->open = ext2_open;
    fs->close = ext2_close;
    fs->read = ext2_read;
    fs->write = ext2_write;
    fs->stat = ext2_stat;
    fs->seek = ext2_seek;
    fs->resolve = ext2_resolve;

    return fs;
}
