#include "dev/block/block.h"

#include "dev/bus.h"
#include "dev/device.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "libk/string.h"

#define PARTITION_TABLE_OFFSET 0x1be
#define PARTITION_TABLE_SIZE   64

struct block_device_private {
    struct partition_table_entry partition_table[4];
};

static int
block_device_read(struct device *device, char *buf, size_t count)
{
    // TODO
    return 0;
}

static int
block_device_write(struct device *device, char *buf, size_t count)
{
    // TODO
    return 0;
}

static int
read_partitions(struct device *device)
{
    struct block_device_private *private = device->bus->private;
    unsigned int device_id = device->id;
    union bus_addr bus_addr = {.val = 0};
    char buf[512];

    int read = device->bus->io_operations->read(private, device_id, bus_addr, buf, 512);
    if (read < 512) {
        return -EIO;
    }

    char *partition_tables_start = buf + PARTITION_TABLE_OFFSET;

    memcpy(private->partition_table, partition_tables_start, PARTITION_TABLE_SIZE);
}

int
block_device_init(struct device *device)
{
    struct block_device_private *private = kzalloc(sizeof(struct block_device_private));
    if (!private) {
        return -ENOMEM;
    }

    strcpy(device->name, "block"); // TODO - Append block number
    device->file_operations.read = block_device_read;
    device->file_operations.write = block_device_write;
    device->private = private;

    int res = read_partitions(device);
    if (res < 0) {
        kfree(private);
        return res;
    }

    return 0;
}

struct partition_table_entry *
block_device_partition_table(struct device *device)
{
    struct block_device_private *private = device->private;
    return private->partition_table;
}