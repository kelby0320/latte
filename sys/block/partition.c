#include "partition.h"

#include "disk.h"
#include "libk/memory.h"

int
partition_read_partition_table(struct disk *disk,
			       struct partition_table_entry *partition_table)
{
    char buf[512];

    int res = disk->read_sectors(disk->device, 0, buf, 1);
    if (res < 0) {
	return res;
    }

    char *partition_table_start = buf + PARTITION_TABLE_OFFSET;

    memcpy(partition_table, partition_table_start, PARTITION_TABLE_SIZE);

    return 0;
}
    
