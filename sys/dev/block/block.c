#include "dev/block/block.h"

#include "bus/ata/ata.h"
#include "dev/device.h"
#include "errno.h"
#include "libk/memory.h"
#include "libk/string.h"

#define PARTITION_TABLE_OFFSET 0x1be
#define PARTITION_TABLE_SIZE   64

static int
block_device_read(struct device *device, size_t offset, char *buf, size_t sector_count)
{
    struct ata_bus *ata_bus = (struct ata_bus *)device->bus;
    unsigned int dev_id = device->id;
    unsigned int lba = offset;

    return ata_bus->read(ata_bus, dev_id, lba, buf, sector_count);
}

static int
block_device_write(struct device *device, size_t offset, const char *buf, size_t size)
{
    struct ata_bus *ata_bus = (struct ata_bus *)device->bus;
    unsigned int dev_id = device->id;
    unsigned int lba = offset;

    return ata_bus->write(ata_bus, dev_id, lba, buf, size);
}

static int
read_partitions(struct block_device *block_device)
{
    struct ata_bus *ata_bus = (struct ata_bus *)block_device->device.bus;
    unsigned int device_id = block_device->device.id;
    char buf[512];

    int res = ata_bus->read(ata_bus, device_id, 0, buf, 1);
    if (res < 0) {
        return res;
    }

    char *partition_tables_start = buf + PARTITION_TABLE_OFFSET;

    memcpy(block_device->partition_table, partition_tables_start, PARTITION_TABLE_SIZE);

    return 0;
}

int
block_device_init(struct block_device *block_device)
{
    strcpy(block_device->device.name, "block"); // TODO - Append block number
    block_device->device.file_operations.read = block_device_read;
    block_device->device.file_operations.write = block_device_write;

    int res = read_partitions(block_device);
    if (res < 0) {
        return res;
    }

    return 0;
}

int
block_device_read_sectors(struct block_device *block_device, unsigned int lba, char *buf,
                          size_t sector_count)
{
    return block_device_read((struct device *)block_device, lba, buf, sector_count);
}

int
block_device_write_sectors(struct block_device *block_device, unsigned int lba, const char *buf,
                           size_t size)
{
    return block_device_write((struct device *)block_device, lba, buf, size);
}