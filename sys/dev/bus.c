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

#define STATIC_PLATFORM_DEVICE_COUNT 7

static struct list_item *bus_list = NULL;

static struct platform_device *
make_platform_device(const char *dev_name, const char *pdev_name,  int num_resources, ...)
{
    struct platform_device *pdev = kzalloc(sizeof(struct platform_device));
    if (!pdev) {
        return NULL;
    }

    make_device(&pdev->device, dev_name);

    int pdev_name_len = strlen(pdev_name);
    pdev->name = kzalloc(pdev_name_len + 1);
    if (!pdev->name) {
        goto err_exit;
    }

    sprintk((char *)pdev->name, pdev_name);

    va_list args;
    va_start(args, num_resources);

    for (int i = 0; i < num_resources; i++) {
	unsigned int base_addr = va_arg(args, unsigned int);
	unsigned int length = va_arg(args, unsigned int);

	struct resource *res = kzalloc(sizeof(struct resource));
	if (!res) {
	    for_each_in_list(struct resource *, pdev->resources, list, resource) {
		kfree(resource);
	    }
	    
	    list_destroy(pdev->resources);
	    kfree((void *)pdev->name);
	    
	    goto err_exit;
	}

	res->base_addr = base_addr;
	res->length = length;

	list_push_back(&pdev->resources, res);
    }

    va_end(args);

    return pdev;
    
err_exit:
    kfree(pdev);
    return NULL;
}

static int
add_platform_devices()
{
    struct platform_device *pdevices[STATIC_PLATFORM_DEVICE_COUNT];
    /* ATA0 Drive 0 */
    pdevices[0] = make_platform_device("ata0-0", "ata", 2, 0x1F0, 8, 0, 1);
    /* ATA0 Drive 1 */
    pdevices[1] = make_platform_device("ata0-1", "ata", 2, 0X1F0, 8, 1, 1);
    /* ATA1 Drive 0 */
    pdevices[2] = make_platform_device("ata1-0", "ata", 2, 0x170, 8, 0, 1);
    /* ATA1 Drive 1 */
    pdevices[3] = make_platform_device("ata1-1", "ata", 2, 0x170, 8, 1, 1);
    /* Console - must come before vga */
    pdevices[4] = make_platform_device("console", "console", 0);
    /* VGA0 */
    pdevices[5] = make_platform_device("vga0", "vga", 1, 0xB8000, 4000);
    /* DEVFS */
    pdevices[6] = make_platform_device("devfs", "devfs", 0);

    for (int i = 0; i < STATIC_PLATFORM_DEVICE_COUNT; i++) {
        if (!pdevices[i]) {
            return -ENOMEM;
        }

        int res = platform_device_register(pdevices[i]);
        if (res < 0) {
            printk("Failed to register platform device: %s\n", pdevices[i]->name);
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
