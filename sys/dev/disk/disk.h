#ifndef DISK_H
#define DISK_H

#include "fs/fs.h"

typedef unsigned int DISK_TYPE;

#define DISK_TYPE_ATA 0

#define DISK_ID_STR_SIZE 8

struct disk;

typedef int (*DISK_READ_BLOCK_FUNCTION)(struct disk *disk, unsigned int lba, int total, void *buf);

struct disk {
    DISK_TYPE type;
    char id[DISK_ID_STR_SIZE];
    void *private;
    struct filesystem *fs;

    DISK_READ_BLOCK_FUNCTION read_block;
};

void
disk_probe_and_init();

struct disk*
disk_get_free_disk();

#endif