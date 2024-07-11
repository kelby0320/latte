#include "dev/platform/platform_device.h"

#include "dev/bus.h"
#include "drivers/driver.h"
#include "drivers/platform/platform_driver.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/print.h"
#include "libk/string.h"

#include <stddef.h>
#include <stdarg.h>

static struct bus platform_bus = {
    .name = "platform",
    .match = platform_match,
    .probe = platform_probe,
    .devices = NULL,
};

int
platform_bus_init()
{
    int res = bus_register(&platform_bus);
    if (res < 0) {
        return res;
    }

    return 0;
}

struct bus *
platform_bus_get_bus()
{
    return &platform_bus;
}

int
platform_match(struct device *dev, struct device_driver *drv)
{
    struct platform_device *pdev = as_platform_device(dev);

    const char *compat = drv->compat;
    const char *pdev_name = pdev->name;

    /* strsep will modify strings so make a copy of compat */
    int compat_size = strlen(compat) + 1;
    char *copy_compat = kzalloc(compat_size);
    if (!copy_compat) {
        return -ENOMEM;
    }

    strcpy(copy_compat, compat);
    char *copy_compat_p = copy_compat;

    int match = -1;
    do {
	char *token = strsep(&copy_compat, ","); /* copy_compat is modified here */
	if (token == NULL) {
	    break;
	}

	match = strcmp(token, pdev_name);
	if (match == 0) {
	    break;
	}
    } while (true);

    kfree(copy_compat_p);	/* Make sure to free copy_compat_p, NOT copy_compat */
    return match;
}

int
platform_probe(struct device *dev)
{
    struct platform_device *pdev = (struct platform_device *)dev;
    struct platform_driver *pdrv = (struct platform_driver *)dev->driver;

    if (pdrv->probe) {
        return pdrv->probe(pdev);
    }

    return 0;
}

int
platform_device_register(struct platform_device *pdev)
{
    pdev->device.bus = &platform_bus;
    list_push_front(&platform_bus.devices, pdev);

    bus_match();

    return 0;
}

struct platform_device *
platform_device_find(const char *name)
{
    for_each_in_list(struct platform_device *, platform_bus.devices, list, pdev) {
	if (strcmp(pdev->name, name) == 0) {
	    return pdev;
	}
    }

    return NULL;
}
