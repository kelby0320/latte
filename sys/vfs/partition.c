#include "vfs/partition.h"

#include "dev/block/block.h"
#include "errno.h"
#include "fs/fs.h"

int
partition_init(struct partition *partition, struct block_device *block_device,
               struct partition_table_entry *part_entry)
{
    // Check that the partition table entry is valid
    if (part_entry->type == 0) {
        return -EINVAL;
    }

    partition->block_device = block_device;
    partition->partition_start = part_entry->lba_start;

    return 0;
}

int
partition_bind_fs(struct partition *partition)
{
    return fs_resolve(partition);
}