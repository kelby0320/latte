#include "fs/bufferedreader.h"

#include "config.h"
#include "dev/block/block.h"
#include "errno.h"
#include "libk/kheap.h"
#include "vfs/partition.h"

int
bufferedreader_new(struct bufferedreader **reader_out, struct partition *partition)
{
    struct bufferedreader *reader = kzalloc(sizeof(struct bufferedreader));
    if (!reader) {
        return -ENOMEM;
    }

    reader->pos = 0;
    reader->partition = partition;

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

    struct block_device *block_device = reader->partition->block_device;

    while (bytes_read < count) {
        // Read a sector of data
        int res = block_device_read_sectors(block_device, sector, buf, 1);
        if (res < 0) {
            return res;
        }

        // Copy data to the out buffer accounting for offset
        int buf_idx = offset;
        int read_limit = (LATTE_SECTOR_SIZE - offset) < (count - bytes_read)
                             ? (LATTE_SECTOR_SIZE - offset)
                             : (count - bytes_read);
        for (int i = 0; i < read_limit; i++) {
            ((char *)out)[out_idx] = buf[buf_idx];
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