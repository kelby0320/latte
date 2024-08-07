#include "fs/ext2/inode.h"

#include "block/buffered_reader.h"
#include "errno.h"
#include "fs/ext2/block_iter.h"
#include "fs/ext2/common.h"
#include "libk/alloc.h"
#include "libk/memory.h"

/**
 * @brief Convert inode number to block group
 *
 * @param fs_private    Pointer to private fs data
 * @param inode         Inode number
 * @return int          Block group number
 */
static inline int
inode_to_block_group(struct ext2_private *fs_private, uint32_t inode)
{
    return (inode - 1) / fs_private->superblock.s_inodes_per_group;
}

/**
 * @brief Read a block group descriptor
 *
 * @param desc_out      Output pointer to block group descriptor structure
 * @param fs_private    Pointer to private fs data
 * @param block_group   Block group number
 * @return int          Status code
 */
static int
ext2_read_block_group_desc(struct ext2_block_group_descriptor **desc_out,
                           struct ext2_private *fs_private, int block_group)
{
    int bg_desc_tbl_start_blk = 1;
    if (fs_private->block_size <= 1024) {
        bg_desc_tbl_start_blk = 2;
    }

    int bg_desc_tbl_start = (bg_desc_tbl_start_blk * fs_private->block_size);
    int bg_desc_start =
        bg_desc_tbl_start + (block_group * sizeof(struct ext2_block_group_descriptor));

    struct ext2_block_group_descriptor *desc = kzalloc(sizeof(struct ext2_block_group_descriptor));
    if (!desc) {
        return -ENOMEM;
    }

    block_buffered_reader_seek(fs_private->reader, bg_desc_start);
    int res =
        block_buffered_reader_read(fs_private->reader, (char *)desc, sizeof(struct ext2_block_group_descriptor));
    if (res < 0) {
        kfree(desc);
        return res;
    }

    *desc_out = desc;
    return 0;
}

/**
 * @brief Read an inode structure from an inode table
 *
 * @param inode_out     Output pointer to inode structure
 * @param fs_private    Pointer to private fs data
 * @param inode_tbl     Inode table block number
 * @param inode_no      Number of the inode to read
 * @return int          Status code
 */
static int
ext2_read_inode_from_tbl(struct ext2_inode **inode_out, struct ext2_private *fs_private,
                         uint32_t inode_tbl, uint32_t inode_no)
{
    int inode_tbl_start = (inode_tbl * fs_private->block_size);
    int inode_start = inode_tbl_start + ((inode_no - 1) * fs_private->superblock.s_inode_size);

    struct ext2_inode *inode = kzalloc(sizeof(struct ext2_inode));
    if (!inode) {
        return -ENOMEM;
    }

    block_buffered_reader_seek(fs_private->reader, inode_start);
    int res = block_buffered_reader_read(fs_private->reader, (char *)inode, sizeof(struct ext2_inode));
    if (res < 0) {
        kfree(inode);
        return res;
    }

    *inode_out = inode;
    return 0;
}

/**
 * @brief Read the next block of data
 *
 * @param iter          Pointer to the block iterator
 * @param fs_private    Ponter to private fs data
 * @param out           Output data buffer
 * @return int          Number of bytes read
 */
static int
ext2_read_block(struct ext2_block_iterator *iter, struct ext2_private *fs_private, char *out)
{
    int block_no = ext2_block_iterator_block_no(iter);
    if (block_no == 0) {
	return -EINVAL; // Invalid block
    }
    
    int data_offset = (block_no * fs_private->block_size);

    block_buffered_reader_seek(fs_private->reader, data_offset);
    return block_buffered_reader_read(fs_private->reader, out, fs_private->block_size);
}

int
ext2_read_inode(struct ext2_inode **inode_out, struct ext2_private *fs_private, uint32_t inode_no)
{
    int block_group = inode_to_block_group(fs_private, inode_no);

    struct ext2_block_group_descriptor *desc;
    int res = ext2_read_block_group_desc(&desc, fs_private, block_group);
    if (res < 0) {
        return res;
    }

    struct ext2_inode *inode;
    res = ext2_read_inode_from_tbl(&inode, fs_private, desc->bg_inode_table, inode_no);
    if (res < 0) {
        return res;
    }

    *inode_out = inode;
    return 0;
}

int
ext2_read_inode_data(struct ext2_private *fs_private, const struct ext2_inode *inode, char *out,
                     size_t count, unsigned int blk_offset, unsigned int byte_offset)
{
    struct ext2_block_iterator iter;
    ext2_block_iterator_init(&iter, fs_private, inode);
    ext2_block_iterator_skip_blocks(&iter, blk_offset);

    char *buf = vzalloc(fs_private->block_size);
    size_t bytes_read = 0;
    size_t bytes_remaining = count;

    while (bytes_read < count) {
        int num_read = ext2_read_block(&iter, fs_private, buf);
	if (num_read != (int)fs_private->block_size) {
	    goto err_read_block;
	}

        size_t bytes_to_copy = fs_private->block_size - byte_offset;
        if (bytes_remaining < bytes_to_copy) {
            bytes_to_copy = bytes_remaining;
        }

        char *dest = out + bytes_read;
        char *src = buf + byte_offset;

        memcpy(dest, src, bytes_to_copy);

        bytes_read += bytes_to_copy;
        bytes_remaining -= bytes_to_copy;
        byte_offset = 0;

        ext2_block_iterator_next(&iter, fs_private);
    }

    vfree(buf);

    return bytes_read;

err_read_block:
    vfree(buf);
    return -EIO;
}
