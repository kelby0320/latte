#ifndef BUFFEREDREADER_H
#define BUFFEREDREADER_H

#include <stddef.h>

struct disk;

struct bufferedreader {
    struct disk *disk;
    unsigned int pos;
};

int
bufferedreader_new(struct bufferedreader **reader_out, struct disk *disk);

int
bufferedreader_seek(struct bufferedreader *reader, unsigned int pos);

int
bufferedreader_read(struct bufferedreader *reader, void *out, int count);

void
bufferedreader_free(struct bufferedreader *reader);

#endif