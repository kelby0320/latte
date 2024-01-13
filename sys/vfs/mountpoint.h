#ifndef MOUNTPOINT_H
#define MOUNTPOINT_H

#include "config.h"
#include "vfs/partition.h"

/**
 * @brief Mountpoint structure
 *
 */
struct mountpoint {
    struct partition partition;
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