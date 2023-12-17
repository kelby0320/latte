#ifndef EXT2_INODE_H
#define EXT2_INODE_H

#include "fs/ext2/common.h"

#include <stddef.h>

struct disk;

/**
 * @brief Read an inode structure
 *
 * @param inode_out     Pointer to output inode structure
 * @param disk          Pointer to the disk
 * @param fs_private    Pointer to private fs data
 * @param inode_no      Inode number
 * @return int          Status code
 */
int
ext2_read_inode(struct inode **inode_out, struct disk *disk, struct ext2_private *fs_private,
                uint32_t inode_no);

/**
 * @brief Read an inodes file data
 *
 * @param fs_private    Pointer to private fs data
 * @param inode         Pointer to the inode
 * @param out           Pointer to the output buffer
 * @param count         Number of bytes to read
 * @param blk_offset    Block offset into inode data
 * @param byte_offset   Byte offset into block to start reading at
 * @return int          Status code
 */
int
ext2_read_inode_data(struct ext2_private *fs_private, const struct inode *inode, char *out,
                     size_t count, unsigned int blk_offset, unsigned int byte_offset);

#endif