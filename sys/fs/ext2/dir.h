#ifndef EXT2_DIR_H
#define EXT2_DIR_H

#include "fs/ext2/common.h"

#include <stdint.h>

struct disk;

/**
 * @brief Find an entry in a directory
 * 
 * @param inode_out     Pointer to the output inode structure for the corresponding entry
 * @param disk          Pointer to the disk
 * @param fs_private    Pointer to private fs data
 * @param dir_inode     Pointer to directory inode
 * @param name          Name of the directory entry to search for
 * @return int          Status code
 */
int
ext2_get_directory_entry(struct inode **inode_out, struct disk *disk, struct ext2_private *fs_private, const struct inode *dir_inode, const char *name);

#endif