#ifndef EXT2_DIR_H
#define EXT2_DIR_H

#include <stdint.h>

struct dir_entry;
struct ext2_inode;
struct ext2_private;
struct ext2_dir_iter;

/**
 * @brief Find an entry in a directory
 *
 * @param inode_out     Pointer to the output inode structure for the
 * corresponding entry
 * @param fs_private    Pointer to private fs data
 * @param dir_inode     Pointer to directory inode
 * @param name          Name of the directory entry to search for
 * @return int          Status code
 */
int
ext2_dir_find_entry(
    struct ext2_inode **inode_out, struct ext2_private *fs_private,
    const struct ext2_inode *dir_inode, const char *name);

int
ext2_dir_next_entry(
    struct ext2_private *fs_private, const struct ext2_inode *dir_inode,
    struct ext2_dir_iter *dir_iter, struct dir_entry *dir_entry_out);

#endif
