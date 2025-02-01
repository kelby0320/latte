#ifndef EXT2_DIR_ITER_H
#define EXT2_DIR_ITER_H

#include "fs/ext2/inode.h"

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Directory iterator
 *
 * This object is used to iterate through directory entries
 *
 */
struct ext2_dir_iter {
    // Directory Inode
    const struct ext2_inode *dir_inode;

    // Data buffer
    char *buf;

    // Data buffer size
    size_t buf_size;

    // Offset into data buffer
    uint32_t buf_offset;

    // Block offset to read from
    uint32_t block_offset;
};

/**
 * @brief Initialze a directory iterator
 *
 * @param iter          Pointer to the directory iterator
 * @param fs_private    Pointer to private fs data
 * @param dir_inode     Pointer to the directory inode
 * @return int          Status code
 */
int
ext2_dir_iter_init(
    struct ext2_dir_iter *iter, struct ext2_private *fs_private,
    const struct ext2_inode *dir_inode);

/**
 * @brief Free a directory iterator
 *
 * @param iter Pointer to the directory iterator
 */
void
ext2_dir_iter_free(struct ext2_dir_iter *iter);

/**
 * @brief Read the next entry in the directory
 *
 * @param iter          Pointer to the directory iterator
 * @param fs_private    Pointer to private fs data
 * @param dir_entry_out Pointer to the output directory entry
 * @return int          Status code
 */
int
ext2_dir_iter_next(
    struct ext2_dir_iter *iter, struct ext2_private *fs_private,
    struct ext2_directory_entry *dir_entry_out);

#endif
