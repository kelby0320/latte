#ifndef EXT2_DIR_H
#define EXT2_DIR_H

#include "fs/ext2/common.h"

#include <stdint.h>

struct disk;

int
ext2_get_directory_entry(struct inode **inode_out, struct disk *disk, struct ext2_private *fs_private, const struct inode *dir_inode, const char *name);

#endif