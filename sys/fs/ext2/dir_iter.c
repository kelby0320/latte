#include "fs/ext2/dir_iter.h"

#include "errno.h"
#include "libk/alloc.h"
#include "libk/memory.h"

int
ext2_dir_iter_init(
    struct ext2_dir_iter *iter, struct ext2_private *fs_private,
    const struct ext2_inode *dir_inode)
{
    char *buf = vzalloc(fs_private->block_size);
    if (!buf) {
        return -ENOMEM;
    }

    iter->dir_inode = dir_inode;
    iter->buf = buf;
    iter->buf_size = fs_private->block_size;
    iter->buf_offset = 0;
    iter->block_offset = 0;

    // Read inital block of data
    int res = ext2_read_inode_data(
        fs_private, iter->dir_inode, iter->buf, iter->buf_size,
        iter->block_offset, 0);
    if (res < 0) {
        return res;
    }

    return 0;
}

void
ext2_dir_iter_free(struct ext2_dir_iter *iter)
{
    vfree(iter->buf);
}

int
ext2_dir_iter_next(
    struct ext2_dir_iter *iter, struct ext2_private *fs_private,
    struct ext2_directory_entry *dir_entry_out)
{
    int res = 0;
    if (iter->buf_offset == fs_private->block_size) {
        // Read next block of data
        iter->block_offset++;
        iter->buf_offset = 0;
        res = ext2_read_inode_data(
            fs_private, iter->dir_inode, iter->buf, iter->buf_size,
            iter->block_offset, 0);
        if (res < 0) {
            memcpy(dir_entry_out, 0, sizeof(struct ext2_directory_entry));
            return res;
        }
    }

    // Parse directory_entry from buffer
    struct ext2_directory_entry *entry =
        as_ext2_directory_entry((iter->buf + iter->buf_offset));
    uint32_t entry_len = entry->rec_len;
    if (entry_len > sizeof(struct ext2_directory_entry)) { // overflow check
        entry_len = sizeof(struct ext2_directory_entry);
    }

    // Copy directory_entry to output
    memcpy(dir_entry_out, entry, entry_len);
    iter->buf_offset += entry->rec_len;

    return entry_len;
}
