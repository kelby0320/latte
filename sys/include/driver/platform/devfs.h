#ifndef DRIVERS_DEVFS_H
#define DRIVERS_DEVFS_H

#include <stddef.h>

struct device;
struct platform_device;

int
devfs_drv_init();

int
devfs_probe(struct platform_device *pdev);

int
devfs_read_sectors(struct device *dev, unsigned int lba, char *buf, size_t count);

#endif
