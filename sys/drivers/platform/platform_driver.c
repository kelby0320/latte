#include "drivers/platform/platform_driver.h"

#include "dev/platform/platform_device.h"
#include "drivers/driver.h"
#include "drivers/platform/ata/ata.h"
#include "drivers/platform/console/console.h"
#include "drivers/platform/devfs/devfs.h"
#include "drivers/platform/vga/vga.h"

#include <stddef.h>

int
platform_driver_init()
{
    ata_drv_init();
    devfs_drv_init();
    vga_drv_init();
    console_drv_init();
    
    return 0;
}

int
platform_driver_register(struct platform_driver *driver)
{
    driver->driver.bus = platform_bus_get_bus();
    driver_register(as_device_driver(driver));

    return 0;
}
