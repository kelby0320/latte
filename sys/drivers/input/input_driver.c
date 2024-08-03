#include "drivers/input/input_driver.h"

#include "dev/input/input_device.h"
#include "drivers/driver.h"
#include "drivers/input/kbd/kbd.h"

int
input_driver_init()
{
    kbd_drv_init();

    return 0;
}

int
input_driver_register(struct input_driver *driver)
{
    driver->driver.bus = input_bus_get_bus();
    driver_register(as_device_driver(driver));

    return 0;
}
