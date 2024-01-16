#include "fs/ext2/inode.h"

#include "config.h"
#include "errno.h"
#include "fs/bufferedreader.h"
#include "fs/ext2/common.h"
#include "libk/kheap.h"
#include "libk/memory.h"

/**
 * @brief Block iterator
 *
 * This object is used to iterate through an inode's data blocks
 *
 */
struct block_iterator {
    // Pointer to inode
    const struct ext2_inode *inode;

    // Block size
    uint32_t block_size;

    // Block index
    int block_idx;

    // Double indirect block index
    int dbl_block_idx;

    // Triple indirect block index
    int tpl_block_idx;

    // Indirect blocks list
    uint32_t *indirect_blocks;

    // Double indirect blocks lisk
    uint32_t *dbl_indirect_blocks;

    // Triple indirect blocks list
    uint32_t *tpl_indirect_blocks;
};

static int
block_iterator_init(struct block_iterator *iter, struct ext2_private *fs_private, const struct ext2_inode *inode)
{
    iter->inode = inode;
    iter->block_size = fs_private->block_size;
    iter->block_idx = 0;
    iter->dbl_block_idx = 0;
    iter->tpl_block_idx = 0;
    iter->indirect_blocks = 0;
    iter->dbl_indirect_blocks = 0;
    iter->tpl_indirect_blocks = 0;

    return 0;
}

/**
 * @brief Skip reading a number of block
 *
 * @param iter      Pointer to block iterator
 * @param offset    Number of blocks to skip
 * @return int      Status code
 */
static int
block_iterator_skip_blocks(struct block_iterator *iter, uint32_t offset)
{
    if (offset > 12) {
        return -EIO; // Only support reading direct block pointers for now
    }

    iter->block_idx = offset;
    return 0;
}

/**
 * @brief Iterate triple indirect blocks
 *
 * @param iter          Pointer to block iterator
 * @param fs_private    Pointer to private fs data
 * @return int          Status code
 */
static int
block_iterator_iterate_tpl(struct block_iterator *iter, struct ext2_private *fs_private)
{
    // TODO
    return 0;
}

/**
 * @brief Iterate doulble indirect blocks
 *
 * @param iter          Pointer to block iterator
 * @param fs_private    Pointer to private fs data
 * @return int          Status code
 */
static int
block_iterator_iterate_dbl(struct block_iterator *iter, struct ext2_private *fs_private)
{
    // TODO
    return 0;
}

/**
 * @brief Iterator indirect blocks
 *
 * @param iter          Pointer to block iterator
 * @param fs_private    Pointer to private fs data
 * @return int          Status code
 */
static int
block_iterator_iterate_indirect(struct block_iterator *iter, struct ext2_private *fs_private)
{
    // TODO
    return 0;
}

/**
 * @brief Set block iterator to iterate indirect blocks
 *
 * @param iter          Pointer to block iterator
 * @param fs_private    Pointer to private fs data
 * @return int          Status code
 */
static int
block_iterator_convert_to_indirect(struct block_iterator *iter, struct ext2_private *fs_private)
{
    // TODO
    return 0;
}

/**
 * @brief Set iterator to read the next block
 *
 * @param iter          Pointer to block iterator
 * @param fs_private    Pointer to private fs data
 * @return int          Status code
 */
static int
block_iterator_iterate(struct block_iterator *iter, struct ext2_private *fs_private)
{
    if (iter->tpl_indirect_blocks) {
        return block_iterator_iterate_tpl(iter, fs_private);
    }

    if (iter->dbl_indirect_blocks) {
        return block_iterator_iterate_dbl(iter, fs_private);
    }

    if (iter->indirect_blocks) {
        return block_iterator_iterate_indirect(iter, fs_private);
    }

    iter->block_idx++;
    if (iter->block_idx == 12) {
        block_iterator_convert_to_indirect(iter, fs_private);
    }

    return 0;
}

/**
 * @brief Get this iterators current data block number
 *
 * @param iter  Pointer to the block iterator
 * @return int  Block number
 */
static int
block_iterator_block_no(struct block_iterator *iter)
{
    return iter->inode->i_block[iter->block_idx];
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
ext2_read_block(struct block_iterator *iter, struct ext2_private *fs_private, char *out)
{
    int block_no = block_iterator_block_no(iter);
    int data_offset = EXT2_FS_START + (block_no * fs_private->block_size);

    bufferedreader_seek(fs_private->reader, data_offset);
    return bufferedreader_read(fs_private->reader, out, fs_private->block_size);
}

/**
 * @brief Convert inode number to block group
 *
 * @param fs_private    Pointer to private fs data
 * @param inode         Inode number
 * @return int          Block group number
 */
static int
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
ext2_read_block_group_desc(struct ext2_block_group_descriptor **desc_out, struct ext2_private *fs_private,
                           int block_group)
{
    int bg_desc_tbl_start_blk = 1;
    if (fs_private->block_size <= 1024) {
        bg_desc_tbl_start_blk = 2;
    }

    int bg_desc_tbl_start = EXT2_FS_START + (bg_desc_tbl_start_blk * fs_private->block_size);
    int bg_desc_start = bg_desc_tbl_start + (block_group * sizeof(struct ext2_block_group_descriptor));

    struct ext2_block_group_descriptor *desc = kzalloc(sizeof(struct ext2_block_group_descriptor));
    if (!desc) {
        return -ENOMEM;
    }

    bufferedreader_seek(fs_private->reader, bg_desc_start);
    int res = bufferedreader_read(fs_private->reader, desc, sizeof(struct ext2_block_group_descriptor));
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
ext2_read_inode_from_tbl(struct ext2_inode **inode_out, struct ext2_private *fs_private, uint32_t inode_tbl,
                         uint32_t inode_no)
{
    int inode_tbl_start = EXT2_FS_START + (inode_tbl * fs_private->block_size);
    int inode_start = inode_tbl_start + ((inode_no - 1) * fs_private->superblock.s_inode_size);

    struct ext2_inode *inode = kzalloc(sizeof(struct ext2_inode));
    if (!inode) {
        return -ENOMEM;
    }

    bufferedreader_seek(fs_private->reader, inode_start);
    int res = bufferedreader_read(fs_private->reader, inode, sizeof(struct ext2_inode));
    if (res < 0) {
        kfree(inode);
        return res;
    }

    *inode_out = inode;
    return 0;
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
ext2_read_inode_data(struct ext2_private *fs_private, const struct ext2_inode *inode, char *out, size_t count,
                     unsigned int blk_offset, unsigned int byte_offset)
{
    struct block_iterator iter;
    block_iterator_init(&iter, fs_private, inode);
    block_iterator_skip_blocks(&iter, blk_offset);

    char *buf = kzalloc(fs_private->block_size);
    int bytes_read = 0;
    int bytes_remaining = count;

    while (bytes_read < bytes_remaining) {
        ext2_read_block(&iter, fs_private, buf);

        int bytes_to_copy = fs_private->block_size - byte_offset;
        if (bytes_remaining < bytes_to_copy) {
            bytes_to_copy = bytes_remaining;
        }

        char *dest = out + bytes_read;
        char *src = buf + byte_offset;

        memcpy(dest, src, bytes_to_copy);

        bytes_read += bytes_to_copy;
        bytes_remaining -= bytes_to_copy;
        byte_offset = 0;

        block_iterator_iterate(&iter, fs_private);
    }

    kfree(buf);

    return bytes_read;
}