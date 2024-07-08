#include "drivers/platform/devfs/devfs.h"

#include "block/disk.h"
#include "config.h"
#include "dev/device.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/print.h"
#include "libk/memory.h"
#include "libk/string.h"

int
devfs_probe(struct platform_device *pdev)
{
    struct disk *disk = kzalloc(sizeof(struct disk));
    if (!disk) {
	return -ENOMEM;
    }

    char *name = kzalloc(8);
    if (!name) {
	goto err_name_alloc;
    }

    sprintk(name, "devfs");

    disk->name = name;
    disk->device = as_device(pdev);
    disk->read_sectors = devfs_read_sectors;

    return disk_register(disk);

err_name_alloc:
    kfree(disk);
    return -ENOMEM;
}

int
devfs_read_sectors(struct device *dev, unsigned int lba, char *buf, size_t count)
{
    // Set the buffer to zero for the prescribed number of sectors
    for (size_t i = 0; i < count; i++) {
	unsigned int offset = i * LATTE_SECTOR_SIZE;
	memset(buf + offset, 0, LATTE_SECTOR_SIZE);
    }

    return 0;
}
