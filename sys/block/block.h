#ifndef BLOCK_H
#define BLOCK_H

struct disk;

struct block {
    unsigned int id;
    struct disk *disk;
    unsigned int lba_start;
};

int
block_add(struct block *block);

#endif
