#include "fs/fs.h"

#include "block/block.h"
#include "config.h"
#include "errno.h"
#include "fs/devfs/devfs.h"
#include "fs/ext2/ext2.h"
#include "fs/fat32/fat32.h"
#include "fs/path.h"
#include "kernel.h"
#include "libk/memory.h"
#include "libk/print.h"
#include "libk/string.h"
#include "vfs/mountpoint.h"

static struct filesystem *filesystems[LATTE_MAX_FILESYSTEMS] = {0};
static int filesystems_len = 0;

/**
 * @brief Add a filesystem to the filesystems list
 *
 * @param filesystem    The filesystem
 * @return int          Status code
 */
static int
fs_insert_filesystem(struct filesystem *filesystem)
{
    if (filesystems_len == LATTE_MAX_FILESYSTEMS) {
        return -EAGAIN;
    }

    struct filesystem **fs = &filesystems[filesystems_len];
    *fs = filesystem;
    filesystems_len++;

    return 0;
}

/**
 * @brief Load filesystem drivers
 *
 */
static int
fs_load()
{
    // fs_insert_filesystem(fat32_init());
    int res = fs_insert_filesystem(ext2_init());
    if (res < 0) {
        printk("Failed to load EXT2 filesystem driver\n");
    }

    res = fs_insert_filesystem(devfs_init());
    if (res < 0) {
        printk("Failed to load DEVFS filesystem driver\n");
    }

    return res;
}

int
fs_init()
{
    return fs_load();
}

int
fs_resolve(struct mountpoint *mountpoint)
{
    for (int i = 0; i < filesystems_len; i++) {
        struct filesystem *filesystem = filesystems[i];
        struct block *block = mountpoint->block;

        void *fs_private = filesystem->resolve(block);
        if (fs_private) {
            mountpoint->filesystem = filesystem;
            mountpoint->fs_private = fs_private;
            return 0;
        }
    }

    return -ENOENT;
}

file_mode_t
fs_get_mode_from_string(const char *str)
{
    file_mode_t mode = FILE_MODE_INVALID;

    if (strncmp(str, "r", 1) == 0) {
        mode = FILE_MODE_READ;
    } else if (strncmp(str, "w", 1) == 0) {
        mode = FILE_MODE_WRITE;
    } else if (strncmp(str, "a", 1) == 0) {
        mode = FILE_MODE_APPEND;
    }

    return mode;
}
