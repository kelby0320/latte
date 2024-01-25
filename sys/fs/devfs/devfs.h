#ifndef DEVFS_H
#define DEVFS_H

struct filesystem;

/**
 * @brief Initialize a devfs filesystem
 *
 * @return struct filesystem*   Pointer to the filesystem
 */
struct filesystem *
devfs_init();

#endif