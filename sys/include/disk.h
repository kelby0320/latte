#ifndef DISK_H
#define DISK_H

#include "partition.h"

#include <stddef.h>

#define DISK_SECTOR_SIZE 512

struct device;

struct disk {
    const char *name;
    struct device *device;
    struct partition_table_entry partition_table[4];
    int (*read_sectors)(struct device *dev, unsigned int lba, char *buf, size_t count);
    int (*write_sectors)(struct device *dev, unsigned int lba, const char *buf, size_t count);
};

int
disk_register(struct disk *disk);

#endif
