#include "fs/ext2/block_iter.h"

#include "buffered_reader.h"
#include "config.h"
#include "errno.h"
#include "fs/ext2/common.h"
#include "libk/alloc.h"
#include "libk/memory.h"

/**
 * @brief Iterate triple indirect blocks
 *
 * @param iter          Pointer to block iterator
 * @param fs_private    Pointer to private fs data
 * @return int          Status code
 */
static int
ext2_block_iterator_next_tpl(
    struct ext2_block_iterator *iter, struct ext2_private *fs_private)
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
ext2_block_iterator_next_dbl(
    struct ext2_block_iterator *iter, struct ext2_private *fs_private)
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
ext2_block_iterator_next_indirect(
    struct ext2_block_iterator *iter, struct ext2_private *fs_private)
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
ext2_block_iterator_convert_to_indirect(
    struct ext2_block_iterator *iter, struct ext2_private *fs_private)
{
    // TODO
    return 0;
}

int
ext2_block_iterator_init(
    struct ext2_block_iterator *iter, struct ext2_private *fs_private,
    const struct ext2_inode *inode)
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

int
ext2_block_iterator_skip_blocks(
    struct ext2_block_iterator *iter, uint32_t offset)
{
    if (offset > 12) {
        return -EIO; // Only support reading direct block pointers for now
    }

    iter->block_idx = offset;
    return 0;
}

int
ext2_block_iterator_next(
    struct ext2_block_iterator *iter, struct ext2_private *fs_private)
{
    if (iter->tpl_indirect_blocks) {
        return ext2_block_iterator_next_tpl(iter, fs_private);
    }

    if (iter->dbl_indirect_blocks) {
        return ext2_block_iterator_next_dbl(iter, fs_private);
    }

    if (iter->indirect_blocks) {
        return ext2_block_iterator_next_indirect(iter, fs_private);
    }

    iter->block_idx++;
    if (iter->block_idx == 12) {
        ext2_block_iterator_convert_to_indirect(iter, fs_private);
    }

    return 0;
}

int
ext2_block_iterator_block_no(struct ext2_block_iterator *iter)
{
    return iter->inode->i_block[iter->block_idx];
}
