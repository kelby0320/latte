#include "drivers/platform/ata/ata.h"

#include "block/block.h"
#include "block/disk.h"
#include "dev/device.h"
#include "dev/platform/platform_device.h"
#include "errno.h"
#include "libk/alloc.h"

int
ata_probe(struct platform_device *pdev)
{
    /* Add new disk for pdev */
    struct disk *disk = kzalloc(sizeof(struct disk));
    if (!disk) {
	return -ENOMEM;
    }

    disk->device = as_device(pdev);
    disk->read_sectors = ata_read_sectors;
    disk->write_sectors = ata_write_sectors;

    return disk_register(disk);
}
