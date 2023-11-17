#ifndef EXT2_INODE_H
#define EXT2_INODE_H

#include "fs/ext2/common.h"

#include <stddef.h>

struct disk;

int
ext2_read_inode(struct inode **inode_out, struct disk *disk, struct ext2_private *fs_private, uint32_t inode_no);

int
ext2_read_inode_data(struct ext2_private *fs_private, const struct inode *inode, char *out, size_t count, unsigned int offset);

#endif