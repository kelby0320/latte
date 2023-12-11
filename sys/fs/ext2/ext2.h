#ifndef EXT2_H
#define EXT2_H

#include "fs/fs.h"

/**
 * @brief Initialize an Ext2 filesystem
 *
 * @return struct filesystem* Pointer to filesystem
 */
struct filesystem *
ext2_init();

#endif