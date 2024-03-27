#include "fs/ext2/dir.h"

#include "errno.h"
#include "fs/ext2/inode.h"
#include "libk/alloc.h"
#include "libk/memory.h"
#include "libk/string.h"

#include <stdint.h>

/**
 * @brief Directory iterator
 *
 * This object is used to iterate through directory entries
 *
 */
struct dir_iter {
    // Directory Inode
    const struct ext2_inode *dir_inode;

    // Data buffer
    char *buf;

    // Data buffer size
    size_t buf_size;

    // Offset into data buffer
    uint32_t buf_offset;

    // Block of data to read from
    uint32_t block_no;
};

/**
 * @brief Initialze a directory iterator
 *
 * @param iter          Pointer to the directory iterator
 * @param fs_private    Pointer to private fs data
 * @param dir_inode     Pointer to the directory inode
 * @return int          Status code
 */
static int
dir_iter_init(struct dir_iter *iter, struct ext2_private *fs_private,
              const struct ext2_inode *dir_inode)
{
    char *buf = kzalloc(fs_private->block_size);
    if (!buf) {
        return -ENOMEM;
    }

    iter->dir_inode = dir_inode;
    iter->buf = buf;
    iter->buf_size = fs_private->block_size;
    iter->buf_offset = 0;
    iter->block_no = 0;

    // Read inital block of data
    int res = ext2_read_inode_data(fs_private, iter->dir_inode, iter->buf, iter->buf_size,
                                   iter->block_no, 0);
    if (res < 0) {
        return res;
    }

    return 0;
}

/**
 * @brief Free a directory iterator
 *
 * @param iter Pointer to the directory iterator
 */
static void
dir_iter_free(struct dir_iter *iter)
{
    kfree(iter->buf);
}

/**
 * @brief Read the next entry in the directory
 *
 * @param iter          Pointer to the directory iterator
 * @param fs_private    Pointer to private fs data
 * @param dir_entry_out Pointer to the output directory entry
 * @return int          Status code
 */
static int
iterate_dir(struct dir_iter *iter, struct ext2_private *fs_private,
            struct ext2_directory_entry *dir_entry_out)
{
    int res = 0;

    if (iter->buf_offset == fs_private->block_size) {
        // Read next block of data
        iter->block_no++;
        iter->buf_offset = 0;
        res = ext2_read_inode_data(fs_private, iter->dir_inode, iter->buf, iter->buf_size,
                                   iter->block_no, 0);
        if (res < 0) {
            return res;
        }
    }

    // Parse directory_entry from buffer
    struct ext2_directory_entry *entry = as_ext2_directory_entry((iter->buf + iter->buf_offset));
    int rec_len = entry->rec_len;
    if (rec_len > sizeof(struct ext2_directory_entry)) {
        rec_len = sizeof(struct ext2_directory_entry);
    }

    // Copy directory_entry to output
    memcpy(dir_entry_out, entry, rec_len);
    iter->buf_offset += rec_len;

    return rec_len;
}

int
ext2_get_directory_entry(struct ext2_inode **inode_out, struct ext2_private *fs_private,
                         const struct ext2_inode *dir_inode, const char *name)
{
    // Confirm dir_inode is a directory
    if (!(dir_inode->i_mode & EXT2_S_IFDIR)) {
        return -EINVAL;
    }

    // Setup a directory iterator
    struct dir_iter iter;
    int res = dir_iter_init(&iter, fs_private, dir_inode);
    if (res < 0) {
        return res;
    }

    // Iterate through directory entries
    struct ext2_directory_entry entry;
    res = iterate_dir(&iter, fs_private, &entry);
    while (res > 0) {
        if (entry.file_type == EXT2_FT_UNKNOWN) {
            // End of directory, entry not found
            res = -ENOENT;
            goto out;
        }

        if (strcmp(entry.name, name) == 0) {
            // Found matching entry
            res = ext2_read_inode(inode_out, fs_private, entry.inode);
            goto out;
        }

        res = iterate_dir(&iter, fs_private, &entry);
    }

out:
    dir_iter_free(&iter);
    return res;
}
