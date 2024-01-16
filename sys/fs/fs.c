#include "fs/fs.h"

#include "config.h"
#include "errno.h"
#include "fs/ext2/ext2.h"
#include "fs/fat32/fat32.h"
#include "fs/path.h"
#include "kernel.h"
#include "libk/memory.h"
#include "libk/string.h"
#include "vfs/partition.h"

static struct filesystem *filesystems[LATTE_MAX_FILESYSTEMS];
static int filesystems_len;

static void *
fs_try_resolve(struct filesystem *filesystem, struct partition *partition)
{
    if (!filesystem) {
        return NULL;
    }

    return filesystem->resolve(partition);
}

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

    return 0;
}

/**
 * @brief Load filesystem drivers
 *
 */
static void
fs_load()
{
    // fs_insert_filesystem(fat32_init());
    fs_insert_filesystem(ext2_init());
}

void
fs_init()
{
    memset(filesystems, 0, sizeof(filesystems));
    filesystems_len = 0;

    fs_load();
}

int
fs_resolve(struct partition *partition)
{
    for (int i = 0; i < LATTE_MAX_FILESYSTEMS; i++) {
        struct filesystem *filesystem = filesystems[i];
        void *fs_private = fs_try_resolve(filesystem, partition);
        if (fs_private) {
            partition->filesystem = filesystem;
            partition->fs_private = fs_private;
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