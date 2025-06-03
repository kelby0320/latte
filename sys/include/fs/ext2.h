#ifndef EXT2_H
#define EXT2_H

struct filesystem;

/**
 * @brief Initialize an Ext2 filesystem
 *
 * @return struct filesystem* Pointer to filesystem
 */
struct filesystem *
ext2_init();

#endif