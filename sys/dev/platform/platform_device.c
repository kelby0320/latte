#include "dev/platform/platform_device.h"

#include "dev/bus.h"
#include "drivers/driver.h"
#include "drivers/platform/platform_driver.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/string.h"

#include <stddef.h>

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
    struct platform_device *pdev = (struct platform_device *)dev;
    struct platform_driver *pdrv = (struct platform_driver *)drv;

    const char *compat = pdrv->driver.compat;
    const char *pdev_name = pdev->name;

    /* strtok will modify strings so make a copy of compat */
    int compat_size = strlen(compat) + 1;
    char *copy_compat = kzalloc(compat_size);
    if (!copy_compat) {
        return -ENOMEM;
    }

    strcpy(copy_compat, compat);

    int match = -1;
    char *tok = strtok(copy_compat, ",");
    while (tok != NULL) {
        match = strcmp(tok, pdev_name);
        if (match == 0) {
            break;
        }
    }

    kfree(copy_compat);
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
}

int
platform_device_register(struct platform_device *pdev)
{
    pdev->device.bus = &platform_bus;
    list_push_front(&platform_bus.devices, pdev);

    bus_match();
    bus_probe();
}
