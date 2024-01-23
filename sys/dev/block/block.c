#include "dev/block/block.h"

#include "bus/ata/ata.h"
#include "dev/device.h"
#include "errno.h"
#include "libk/memory.h"
#include "libk/print.h"
#include "libk/string.h"

#define PARTITION_TABLE_OFFSET 0x1be
#define PARTITION_TABLE_SIZE   64

static unsigned int block_device_number = 0;

/**
 * @brief Block device read file operation
 *
 * @param device        Pointer to the device
 * @param offset        LBA to read from
 * @param buf           Read buffer
 * @param sector_count  Number of sectors to read
 * @return int          Status code
 */
static int
block_device_read(struct device *device, size_t offset, char *buf, size_t sector_count)
{
    struct block_device *block_device = (struct block_device *)device;
    struct ata_bus *ata_bus = (struct ata_bus *)device->bus;
    unsigned int drive_no = block_device->drive_no;
    unsigned int lba = offset;

    return ata_bus->read(ata_bus, drive_no, lba, buf, sector_count);
}

/**
 * @brief Block device write file operation
 *
 * @param device    Pointer to the device
 * @param offset    LBA to write to
 * @param buf       Buffer to write
 * @param size      Size of the buffer
 * @return int      Status code
 */
static int
block_device_write(struct device *device, size_t offset, const char *buf, size_t size)
{
    struct block_device *block_device = (struct block_device *)device;
    struct ata_bus *ata_bus = (struct ata_bus *)device->bus;
    unsigned int drive_no = block_device->drive_no;
    unsigned int lba = offset;

    return ata_bus->write(ata_bus, drive_no, lba, buf, size);
}

int
block_device_init(struct block_device *block_device, block_device_type_t type,
                  unsigned int drive_no, unsigned int lba_offset)
{
    if (type > BLOCK_DEVICE_TYPE_PART) {
        return -EINVAL;
    }

    if (type == BLOCK_DEVICE_TYPE_DISK) {
        if (lba_offset != 0) {
            return -EINVAL;
        }
    }

    sprintk(block_device->device.name, "block%d", block_device_number++);
    block_device->type = type;
    block_device->drive_no = drive_no;
    block_device->lba_offset = lba_offset;
    block_device->device.file_operations.read = block_device_read;
    block_device->device.file_operations.write = block_device_write;

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

int
block_device_read_partitions(struct block_device *block_device,
                             struct partition_table_entry *partition_table)
{
    // The partition table can only be read from the disk itself
    if (block_device->type != BLOCK_DEVICE_TYPE_DISK) {
        return -EINVAL;
    }

    struct ata_bus *ata_bus = (struct ata_bus *)block_device->device.bus;
    unsigned int drive_no = block_device->drive_no;
    char buf[512];

    int res = ata_bus->read(ata_bus, drive_no, 0, buf, 1);
    if (res < 0) {
        return res;
    }

    char *partition_tables_start = buf + PARTITION_TABLE_OFFSET;

    memcpy(partition_table, partition_tables_start, PARTITION_TABLE_SIZE);

    return 0;
}