#ifndef EXT2_BLOCK_ITER_H
#define EXT2_BLOCK_ITER_H

#include "fs/ext2/common.h"

#include <stdint.h>

/**
 * @brief Block iterator
 *
 * This object is used to iterate through an inode's data blocks
 *
 */
struct ext2_block_iterator {
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

/**
 * @brief Initialize a block iterator
 *
 * @param iter          Pointer to block iterator
 * @param fs_private    Pointer to private fs data
 * @param inode         Pointer to inode
 * @return int          Status code
 */
int
ext2_block_iterator_init(
    struct ext2_block_iterator *iter, struct ext2_private *fs_private,
    const struct ext2_inode *inode);

/**
 * @brief Skip reading a number of block
 *
 * @param iter      Pointer to block iterator
 * @param offset    Number of blocks to skip
 * @return int      Status code
 */
int
ext2_block_iterator_skip_blocks(
    struct ext2_block_iterator *iter, uint32_t offset);

/**
 * @brief Set iterator to read the next block
 *
 * @param iter          Pointer to block iterator
 * @param fs_private    Pointer to private fs data
 * @return int          Status code
 */
int
ext2_block_iterator_next(
    struct ext2_block_iterator *iter, struct ext2_private *fs_private);

/**
 * @brief Get this iterators current data block number
 *
 * @param iter  Pointer to the block iterator
 * @return int  Block number
 */
int
ext2_block_iterator_block_no(struct ext2_block_iterator *iter);

#endif
