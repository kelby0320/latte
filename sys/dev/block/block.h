#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include "dev/device.h"

#include <stddef.h>
#include <stdint.h>

#define BLOCK_DEVICE_TYPE_DISK 0
#define BLOCK_DEVICE_TYPE_PART 1

/**
 * @brief MBR Partition Table Entry
 *
 */
struct partition_table_entry {
    uint8_t attrib;
    uint8_t chs_start[3];
    uint8_t type;
    uint8_t chs_end[3];
    uint32_t lba_start;
    uint32_t sector_count;
} __attribute__((packed));

typedef unsigned int block_device_type_t;

/**
 * @brief Block device structure
 *
 */
struct block_device {
    struct device device;
    block_device_type_t type;
    unsigned int drive_no;
    unsigned int lba_offset;
};

/**
 * @brief Initialize a block device
 *
 * @param block_device  Pointer to a block device
 * @param drive_no      Driver number
 * @return int          Status code
 */
int
block_device_init(struct block_device *block_device, block_device_type_t type,
                  unsigned int drive_no, unsigned int lba_offset);

/**
 * @brief Read sectors from a block device
 *
 * @param block_device  Pointer to the block device
 * @param lba           LBA to read from
 * @param buf           Buffer to read into
 * @param sector_count     Sector count to read
 * @return int
 */
int
block_device_read_sectors(struct block_device *block_device, unsigned int lba, char *buf,
                          size_t sector_count);

/**
 * @brief Write data to a block device
 *
 * @param block_device  Pointer to the block device
 * @param lba           LBA to write to
 * @param buf           Buffer to write
 * @param size          Size of the buffer
 * @return int
 */
int
block_device_write_sectors(struct block_device *block_device, unsigned int lba, const char *buf,
                           size_t size);

/**
 * @brief Read the partition table from a block device (must by a block device of type DISK)
 *
 * @param block_device      Pointer to the block device
 * @param partition_table   Pointer to the partition table
 * @return int              Status code
 */
int
block_device_read_partitions(struct block_device *block_device,
                             struct partition_table_entry *partition_table);

#endif