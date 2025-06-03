#ifndef BLOCK_BUFFERED_READER_H
#define BLOCK_BUFFERED_READER_H

struct block;

struct block_buffered_reader {
    struct block *block;
    unsigned int offset;
};

void
block_buffered_reader_init(
    struct block_buffered_reader *reader, struct block *block);

int
block_buffered_reader_seek(
    struct block_buffered_reader *reader, unsigned int pos);

int
block_buffered_reader_read(
    struct block_buffered_reader *reader, char *out, unsigned int count);

#endif
