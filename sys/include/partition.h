#ifndef PARTITION_H
#define PARTITION_H

#include <stdint.h>

struct disk;

#define PARTITION_TABLE_OFFSET 0x1be
#define PARTITION_TABLE_SIZE   64
#define PARTITION_ATTRIB_BOOTABLE 0x80

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

int
partition_read_partition_table(struct disk *disk,
			       struct partition_table_entry *partition_table);

#endif
