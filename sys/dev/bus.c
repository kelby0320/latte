#include "dev/bus.h"

#include "dev/device.h"
#include "dev/platform/platform_device.h"
#include "dev/input/input_device.h"
#include "drivers/driver.h"
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
            bus->probe(dev);
        }
    }
}

int
bus_init()
{
    platform_bus_init();
    input_bus_init();
    
    platform_add_devices();
    input_add_devices();

    return 0;
}

int
bus_register(struct bus *bus)
{
    return list_push_back(&bus_list, bus);
}

int
bus_match()
{
    for_each_in_list(const struct bus *, bus_list, list, bus)
    {
        bus_match_devices(bus);
    }
}

int
bus_probe()
{
    for_each_in_list(const struct bus *, bus_list, list, bus)
    {
        bus_probe_devices(bus);
    }
}
