#ifndef DISK_H
#define DISK_H

#include "config.h"
#include "fs/fs.h"

typedef unsigned int DISK_TYPE;

#define DISK_TYPE_ATA 0

struct disk;

typedef int (*DISK_READ_SECTORS_FUNCTION)(struct disk *disk, unsigned int lba, int total, void *buf);

struct disk {
    DISK_TYPE type;

    DISK_READ_SECTORS_FUNCTION read_sectors;

    char id[LATTE_DISK_ID_MAX_SIZE];

    void *disk_private;

    struct filesystem *fs;
    void *fs_private;
};

void
disk_probe_and_init();

struct disk*
disk_get_free_disk();

#endif