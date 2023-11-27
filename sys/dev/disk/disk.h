#ifndef DISK_H
#define DISK_H

#include "config.h"
#include "fs/fs.h"

typedef unsigned int DISK_TYPE;

#define DISK_TYPE_ATA 0

struct disk;

/**
 * @brief Prototype definition of the function used to read sectors from a disk.
 * 
 * @param disk  Pointer to disk structure
 * @param lba   Logical Block Address to read
 * @param total Number of sectors to read
 * @param buf   Output buffer to write data to
 * @return int  Status code
 * 
 */
typedef int (*DISK_READ_SECTORS_FUNCTION)(struct disk *disk, unsigned int lba, int total, void *buf);

/**
 * @brief Disk type definition.
 * 
 */
struct disk {
    // The type of the disk
    DISK_TYPE type;

    // Id string of the disk
    char id[LATTE_DISK_ID_MAX_SIZE];

    // Pointer to a function to read sectors from the disk
    DISK_READ_SECTORS_FUNCTION read_sectors;

    // Pointer to private data for the disk
    void *disk_private;

    // Pointer to the filesystem of the disk
    struct filesystem *fs;

    // Pointer to private filesystem data
    void *fs_private;
};

/**
 * @brief Find and initialize disks.
 * 
 */
void
disk_probe_and_init();


/**
 * @brief Allocate a new disk structure.
 * 
 * A maximum of LATTE_MAX_DISKS disks can be allocated.
 * 
 * @return struct disk* 
 */
struct disk*
disk_get_free_disk();

#endif