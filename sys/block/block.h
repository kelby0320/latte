#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>

struct disk;

struct block {
    const char *name;
    struct disk *disk;
    unsigned int lba_start;
    bool bootable;
};

int
block_add(struct block *block);

struct block *
block_find(bool (*predicate)(struct block *));

#endif
