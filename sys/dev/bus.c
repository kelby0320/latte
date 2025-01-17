#include "bus.h"

#include "device.h"
#include "dev/platform_device.h"
#include "dev/input_device.h"
#include "driver.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/print.h"
#include "libk/string.h"

#include <stddef.h>

static struct list_item *bus_list = NULL;

static void
bus_match_devices(const struct bus *bus)
{
    for_each_in_list(struct device *, bus->devices, list, dev)
    {
        // Skip devices that are already bound to a driver
        if (dev->driver) {
            continue;
        }

        struct device_driver *drv = NULL;
        while ((drv = driver_find_next(drv)) != NULL) {
            // Skip driver if it is for on this bus
            if (drv->bus != bus) {
                continue;
            }

            if (bus->match(dev, drv) == 0) {
                dev->driver = drv;
		printk("driver %s bound to device %s\n", drv->name, dev->name);
                break;
            }
        }
    }
}

static void
bus_probe_devices(const struct bus *bus)
{
    for_each_in_list(struct device *, bus->devices, list, dev)
    {
        if (dev->driver) {
	    printk("driver %s probing device %s\n", dev->driver->name, dev->name);
            bus->probe(dev);
        }
    }
}

int
bus_init()
{
    platform_bus_init();
    platform_add_devices();

    return 0;
}

int
bus_register(struct bus *bus)
{
    int res = list_push_back(&bus_list, bus);
    if (res < 0) {
	printk("Failed to register new bus, %s\n", bus->name);
	return res;
    }
    
    printk("Registered new bus %s\n", bus->name);
    return 0;
}

int
bus_match()
{
    for_each_in_list(const struct bus *, bus_list, list, bus)
    {
        bus_match_devices(bus);
    }

    return 0;
}

int
bus_probe()
{
    for_each_in_list(const struct bus *, bus_list, list, bus)
    {
        bus_probe_devices(bus);
    }

    return 0;
}
