#include "buffered_reader.h"

#include "block.h"
#include "device.h"
#include "disk.h"

void
block_buffered_reader_init(
    struct block_buffered_reader *reader, struct block *block)
{
    reader->block = block;
    reader->offset = 0;
}

int
block_buffered_reader_seek(
    struct block_buffered_reader *reader, unsigned int offset)
{
    reader->offset = offset;
    return 0;
}

int
block_buffered_reader_read(
    struct block_buffered_reader *reader, char *out, unsigned int count)
{
    unsigned int start_offset = reader->block->lba_start * DISK_SECTOR_SIZE;
    unsigned int sector = (start_offset + reader->offset) / DISK_SECTOR_SIZE;
    unsigned int offset = (start_offset + reader->offset) % DISK_SECTOR_SIZE;

    char buf[DISK_SECTOR_SIZE];
    unsigned int bytes_read = 0;
    int out_idx = 0;

    struct disk *disk = reader->block->disk;
    struct device *dev = disk->device;

    while (bytes_read < count) {
        // Read a sector of data
        int res = disk->read_sectors(dev, sector, buf, 1);
        if (res < 0) {
            return res;
        }

        // Copy data to the out buffer accounting for offset
        int buf_idx = offset;
        int read_limit = (DISK_SECTOR_SIZE - offset) < (count - bytes_read)
                             ? (DISK_SECTOR_SIZE - offset)
                             : (count - bytes_read);
        for (int i = 0; i < read_limit; i++) {
            out[out_idx] = buf[buf_idx];
            out_idx++;
            buf_idx++;
            bytes_read++;
        }

        offset = 0; // Set offset to 0 after reading the first sector
        sector++;
    }

    return bytes_read;
}
