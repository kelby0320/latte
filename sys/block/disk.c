#include "block/disk.h"

#include "block/block.h"
#include "block/partition.h"
#include "dev/device.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/print.h"

static unsigned int next_id = 0;
static struct list_item *disk_list = NULL;

int
disk_register(struct disk *disk)
{
    disk->id = next_id++;
    int res = list_push_back(&disk_list, disk);
    if (res < 0) {
	return res;
    }
    
    // Read the disk's partition table
    struct partition_table_entry *partition_table = disk->partition_table;
    res = partition_read_partition_table(disk, partition_table);
    if (res < 0) {
	return -EIO;
    }

    // Add a block device for each valid partition on the disk
    for (int i = 0; i < 4; i++) {
	if (partition_table[i].sector_count == 0) {
            continue; // Table entry doesn't correspond to a partition
        }

	unsigned int lba_start = partition_table[i].lba_start;
	printk("Found partition starting at %d on %s\n", lba_start, disk->device->name);
	
	struct block *block = kzalloc(sizeof(struct block));
	if (!block) {
	    return -ENOMEM;
	}

	block->disk = disk;
	block->lba_start = lba_start;
	
	res = block_add(block);
	if (res < 0) {
	    return res;
	}
    }
}
