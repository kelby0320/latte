#include "drivers/platform/ata/ata.h"

#include "block/block.h"
#include "block/disk.h"
#include "dev/device.h"
#include "dev/platform/platform_device.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/print.h"

static unsigned int ata_disk_no = 0;

int
ata_probe(struct platform_device *pdev)
{
    /* Add new disk for pdev */
    struct disk *disk = kzalloc(sizeof(struct disk));
    if (!disk) {
	return -ENOMEM;
    }

    char *name = kzalloc(8);
    if (!name) {
	goto err_name_alloc;
    }
    
    sprintk(name, "hdd%d", ata_disk_no++);

    disk->name = name;
    disk->device = as_device(pdev);
    disk->read_sectors = ata_read_sectors;
    disk->write_sectors = ata_write_sectors;

    return disk_register(disk);

err_name_alloc:
    kfree(disk);
    return -ENOMEM;
}
