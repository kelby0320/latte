#include "driver/platform_driver.h"

#include "dev/platform_device.h"
#include "driver.h"
#include "driver/platform/ata.h"
#include "driver/platform/console.h"
#include "driver/platform/devfs.h"
#include "driver/platform/kbd.h"
#include "driver/platform/vga.h"

#include <stddef.h>

int
platform_driver_init()
{
    ata_drv_init();
    devfs_drv_init();
    vga_drv_init();
    console_drv_init();
    kbd_drv_init();
    
    return 0;
}

int
platform_driver_register(struct platform_driver *driver)
{
    driver->driver.bus = platform_bus_get_bus();
    driver_register(as_device_driver(driver));

    return 0;
}
