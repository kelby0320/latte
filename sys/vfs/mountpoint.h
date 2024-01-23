#ifndef MOUNTPOINT_H
#define MOUNTPOINT_H

#include "config.h"

struct block_device;
struct filesystem;

/**
 * @brief Mountpoint structure
 *
 */
struct mountpoint {
    struct block_device *block_device;
    struct filesystem *filesystem;
    void *fs_private;
    char path[LATTE_MAX_PATH_LEN];
};

/**
 * @brief Initialize the mount point table
 *
 */
void
mountpoint_init();

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