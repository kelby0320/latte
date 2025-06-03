#ifndef MOUNTPOINT_H
#define MOUNTPOINT_H

#include "config.h"

struct block;
struct filesystem;

/**
 * @brief Mountpoint structure
 *
 */
struct mountpoint {
    // Pointer to the block device
    struct block *block;

    // Pointer to the filesystem
    struct filesystem *filesystem;

    // Pointer to filesystem private data
    void *fs_private;

    // Mountpoint path
    char path[LATTE_MAX_PATH_LEN];
};

/**
 * @brief Add a mountpoint
 *
 * @param mountpoint    Pointer to the mountpoint
 * @return int          Status code
 */
int
mountpoint_add(struct mountpoint *mountpoint);

/**
 * @brief Lookup a mountpoint from a filename
 *
 * @param filename              The filename
 * @return struct mountpoint*   Pointer to a mountpoint if found, NULL otherwise
 */
struct mountpoint *
mountpoint_find(const char *filename);

#endif
