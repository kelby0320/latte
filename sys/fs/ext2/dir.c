#include "fs/ext2/dir.h"

#include "errno.h"
#include "file_descriptor.h"
#include "fs/ext2/dir_iter.h"
#include "fs/ext2/inode.h"
#include "libk/alloc.h"
#include "libk/memory.h"
#include "libk/string.h"

#include <stdint.h>

int
ext2_dir_find_entry(
    struct ext2_inode **inode_out, struct ext2_private *fs_private,
    const struct ext2_inode *dir_inode, const char *name)
{
    // Confirm dir_inode is a directory
    if (!(dir_inode->i_mode & EXT2_S_IFDIR)) {
        return -EINVAL;
    }

    // Setup a directory iterator
    struct ext2_dir_iter iter;
    int res = ext2_dir_iter_init(&iter, fs_private, dir_inode);
    if (res < 0) {
        return res;
    }

    // Iterate through directory entries
    struct ext2_directory_entry entry;
    res = ext2_dir_iter_next(&iter, fs_private, &entry);
    while (res > 0) {
        if (entry.file_type == EXT2_FT_UNKNOWN) {
            // End of directory, entry not found
            res = -ENOENT;
            goto out;
        }

        size_t name_len = strlen(name);
        if (entry.name_len == name_len &&
            memcmp(entry.name, name, name_len) == 0) {
            // Found matching entry
            res = ext2_read_inode(inode_out, fs_private, entry.inode);
            goto out;
        }

        res = ext2_dir_iter_next(&iter, fs_private, &entry);
    }

out:
    ext2_dir_iter_free(&iter);
    return res;
}

int
ext2_dir_next_entry(
    struct ext2_private *fs_private, const struct ext2_inode *dir_inode,
    struct ext2_dir_iter *dir_iter, struct dir_entry *dir_entry_out)
{
    // Confirm dir_inode is a directory
    if (!(dir_inode->i_mode & EXT2_S_IFDIR)) {
        return -EINVAL;
    }

    struct ext2_directory_entry entry;
    int res = ext2_dir_iter_next(dir_iter, fs_private, &entry);
    if (res < 0) {
        memcpy(dir_entry_out, 0, sizeof(struct dir_entry));
        return res;
    }

    dir_entry_out->inode = entry.inode;
    strcpy(dir_entry_out->name, entry.name);

    return 0;
}
