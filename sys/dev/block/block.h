#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <stddef.h>
#include <stdint.h>

struct device;

struct partition_table_entry {
    uint8_t attrib;
    uint8_t chs_start[3];
    uint8_t type;
    uint8_t chs_end[3];
    uint32_t lba_start;
    uint32_t sector_count;
} __attribute__((packed));

int
block_device_init(struct device *device);

struct partition_table_entry *
block_device_partition_table(struct device *device);

#endif