#include "bus/virtual.h"

#include "bus/dev/dev.h"
#include "bus/devfs/devfs.h"
#include "libk/print.h"

int
virtual_bus_init()
{
    int res = devfs_bus_init();
    if (res < 0) {
        printk("Failed to initialize devfs bus\n");
    }

    res = dev_bus_init();
    if (res < 0) {
        printk("Failed to initialize dev bus\n");
    }

    return 0;
}