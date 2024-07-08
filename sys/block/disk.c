#include "block/disk.h"

#include "block/block.h"
#include "block/partition.h"
#include "dev/device.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/print.h"
#include "libk/string.h"

static struct list_item *disk_list = NULL;

/**
 * @brief Add a block for the disk itself
 *
 * @param disk    Pointer to the disk
 * @return int    Status code
 */
static int
add_disk_block(struct disk *disk)
{
    int res = 0;
    
    struct block *disk_block = kzalloc(sizeof(struct block));
    if (!disk_block) {
	return -ENOMEM;
    }

    char *disk_block_name = kzalloc(16);
    if (!disk_block_name) {
	res = -ENOMEM;
	goto err_name_alloc;
    }

    strcpy(disk_block_name, disk->name);

    disk_block->name = disk_block_name;
    disk_block->disk = disk;

    res = block_add(disk_block);
    if (res < 0) {
        goto err_block_add;
    }

    return res;

err_block_add:
    kfree(disk_block_name);

err_name_alloc:
    kfree(disk_block);
    return res;
}

/**
 * @brief Add a block for each partition on the disk
 *
 * @param disk    Pointer to the disk
 * @return int    Status code
 */
static int
add_disk_partitions(struct disk *disk)
{
    // Read the disk's partition table
    struct partition_table_entry *partition_table = disk->partition_table;
    int res = partition_read_partition_table(disk, partition_table);
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

	bool bootable = partition_table[i].attrib & PARTITION_ATTRIB_BOOTABLE;
	
	struct block *block = kzalloc(sizeof(struct block));
	if (!block) {
	    return -ENOMEM;
	}

	char *name = kzalloc(8);
	if (!name) {
	    kfree(block);
	    return -ENOMEM;
	}

	sprintk(name, "%sp%d", disk->name, i);

	block->name = name;
	block->disk = disk;
	block->lba_start = lba_start;
	block->bootable = bootable;
	
	res = block_add(block);
	if (res < 0) {
	    return res;
	}
    }

    return 0;
}

int
disk_register(struct disk *disk)
{
    int res = list_push_back(&disk_list, disk);
    if (res < 0) {
	return res;
    }
    
    res = add_disk_block(disk);
    if (res < 0) {
	return res;
    }

    res = add_disk_partitions(disk);
    if (res < 0) {
	return res;
    }

    return 0;
}
