#include "drivers/platform/platform_driver.h"

#include "dev/platform/platform_device.h"
#include "drivers/driver.h"

#include <stddef.h>

int
platform_driver_register(struct platform_driver *driver)
{
    driver->driver.bus = platform_bus_get_bus();
    driver_register((struct device_driver *)driver);

    return 0;
}
