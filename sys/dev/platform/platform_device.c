#include "dev/platform_device.h"

#include "bus.h"
#include "driver.h"
#include "driver/platform_driver.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/print.h"
#include "libk/string.h"

#include <stdarg.h>
#include <stddef.h>

#define STATIC_PLATFORM_DEVICE_COUNT 8

static struct bus platform_bus = {
    .name = "platform",
    .match = platform_match,
    .probe = platform_probe,
    .devices = NULL,
};

static struct platform_device *
make_platform_device(
    const char *dev_name, const char *pdev_name, int num_resources, ...)
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
            for_each_in_list(struct resource *, pdev->resources, list, resource)
            {
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
platform_add_devices()
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
    /* KBD0 */
    pdevices[7] = make_platform_device("kbd0", "kbd", 0);

    for (int i = 0; i < STATIC_PLATFORM_DEVICE_COUNT; i++) {
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
        char *token =
            strsep(&copy_compat, ","); /* copy_compat is modified here */
        if (token == NULL) {
            break;
        }

        match = strcmp(token, pdev_name);
        if (match == 0) {
            break;
        }
    } while (true);

    kfree(copy_compat_p); /* Make sure to free copy_compat_p, NOT copy_compat */
    return match;
}

int
platform_probe(struct device *dev)
{
    struct platform_driver *pdrv = as_platform_driver(dev->driver);

    if (pdrv->probe) {
        return pdrv->probe(as_platform_device(dev));
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
    for_each_in_list(struct platform_device *, platform_bus.devices, list, pdev)
    {
        if (strcmp(pdev->name, name) == 0) {
            return pdev;
        }
    }

    return NULL;
}
