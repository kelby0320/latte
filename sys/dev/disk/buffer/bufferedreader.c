#include "dev/disk/buffer/bufferedreader.h"

#include "config.h"
#include "dev/disk/disk.h"
#include "errno.h"
#include "libk/kheap.h"

int
bufferedreader_new(struct bufferedreader **reader_out, struct disk *disk)
{
    struct bufferedreader *reader = kzalloc(sizeof(struct bufferedreader));
    if (!reader) {
        return -ENOMEM;
    }

    reader->pos = 0;
    reader->disk = disk;

    *reader_out = reader;
    return 0;
}

int
bufferedreader_seek(struct bufferedreader *reader, unsigned int pos)
{
    reader->pos = pos;
    return 0;
}

int
bufferedreader_read(struct bufferedreader *reader, void *out, int count)
{
    unsigned int sector = reader->pos / LATTE_SECTOR_SIZE;
    unsigned int offset = reader->pos % LATTE_SECTOR_SIZE;

    char buf[LATTE_SECTOR_SIZE];
    unsigned int bytes_read = 0;
    int out_idx = 0;
    

    while (bytes_read < count) {
        // Read a sector of data
        int res = reader->disk->read_sectors(reader->disk, sector, 1, buf);
        if (res < 0) {
            return res;
        }

        // Copy data to the out buffer accounting for offset
        int buf_idx = offset;
        int read_limit = (LATTE_SECTOR_SIZE - offset) < (count - bytes_read) ? (LATTE_SECTOR_SIZE - offset) : (count - bytes_read);
        for (int i = 0; i < read_limit; i++) {
            ((char*)out)[out_idx] = buf[buf_idx];
            out_idx++;
            buf_idx++;
            bytes_read++;
        }

        offset = 0; // Set offset to 0 after reading the first sector
        sector++;
    }

    return bytes_read;
}

void
bufferedreader_free(struct bufferedreader *reader)
{
    kfree(reader);
}