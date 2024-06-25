#include "dev/bus.h"

#include "dev/device.h"
#include "dev/platform/platform_device.h"
#include "drivers/driver.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/print.h"
#include "libk/string.h"

#include <stddef.h>

static struct list_item *bus_list = NULL;

static struct platform_device *
make_platform_device(const char *name, unsigned int base_addr, unsigned int length)
{
    struct platform_device *pdev = kzalloc(sizeof(struct platform_device));
    if (!pdev) {
        return NULL;
    }

    make_device(&pdev->device, name);

    int name_len = strlen(name);
    pdev->name = kzalloc(name_len + 1);
    if (!pdev->name) {
        goto err_name;
    }

    sprintk((char *)pdev->name, "platform_device");

    struct resource *res = kzalloc(sizeof(struct resource));
    if (!res) {
	goto err_res;
    }

    res->base_addr = base_addr;
    res->length = length;

    list_push_front(&pdev->resources, res);

    return pdev;

err_res:
    kfree(pdev->name);
    
err_name:
    kfree(pdev);
    return NULL;
}

static int
add_platform_devices()
{
    struct platform_device *pdevices[3];
    pdevices[0] = make_platform_device("ata0", 0x1F0, 8);
    pdevices[1] = make_platform_device("ata1", 0x170, 8);
    pdevices[2] = make_platform_device("vga0", 0xB8000, 4000);

    for (int i = 0; i < 3; i++) {
        if (!pdevices[i]) {
            return -ENOMEM;
        }

        int res = platform_device_register(pdevices[i]);
        if (res < 0) {
            printk(
                "Failed to register platform device: %s\n", pdevices[i]->name);
        }
    }

    return 0;
}

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
    // Statically initialize platform devices
    platform_bus_init();
    add_platform_devices();

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
