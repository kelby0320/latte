#include "dev/input/input_device.h"

#include "dev/bus.h"
#include "drivers/driver.h"
#include "drivers/input/input_driver.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/memory.h"
#include "libk/print.h"
#include "libk/string.h"

#include <stdbool.h>

#define DEFAULT_KBD_BUFFER_LEN 512

static struct bus input_bus = {
    .name = "input",
    .match = input_match,
    .probe = input_probe,
    .devices = NULL
};

int
input_bus_init()
{
    int res = bus_register(&input_bus);
    if (res < 0) {
	return res;
    }

    return 0;
}

struct bus *
input_bus_get_bus()
{
    return &input_bus;
}

int
input_add_devices()
{
    struct input_device *kbd_idev = kzalloc(sizeof(struct input_device));
    if (!kbd_idev) {
	return -ENOMEM;
    }

    make_device(&kbd_idev->device, "kbd0");

    int idev_name_len = strlen("kbd");
    kbd_idev->name = kzalloc(idev_name_len + 1);
    if (!kbd_idev->name) {
	goto err_exit;
    }

    sprintk((char *)kbd_idev->name, "kbd");

    kbd_idev->type = INPUT_DEVICE_TYPE_KEYBOARD;

    kbd_idev->buffer = kzalloc(DEFAULT_KBD_BUFFER_LEN);
    if (!kbd_idev->buffer) {
	goto err_exit;
    }

    kbd_idev->buffer_size = DEFAULT_KBD_BUFFER_LEN;
    kbd_idev->buffer_len = 0;
    kbd_idev->buffer_loc = 0;

    int res = input_device_register(kbd_idev);
    if (res < 0) {
	printk("Failed to register input device : %s\n", kbd_idev->name);
    }

    return 0;

err_exit:
    if (kbd_idev->name) {
	kfree((void *)kbd_idev->name);
    }

    if (kbd_idev->buffer) {
	kfree(kbd_idev->buffer);
    }

    return -ENOMEM;
}

int
input_match(struct device *dev, struct device_driver *drv)
{
    struct input_device *idev = as_input_device(dev);

    const char *compat = drv->compat;
    const char *idev_name = idev->name;

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

	match = strcmp(token, idev_name);
	if (match == 0) {
	    break;
	}
    } while (true);

    kfree(copy_compat_p);	/* Make sure to free copy_compat_p, NOT copy_compat */
    return match;
}

int
input_probe(struct device *dev)
{
    struct input_device *idev = as_input_device(dev);
    struct input_driver *idrv = as_input_driver(dev->driver);

    if (idrv->probe) {
	return idrv->probe(idev);
    }

    return 0;
}

int
input_device_register(struct input_device *idev)
{
    idev->device.bus = &input_bus;
    list_push_front(&input_bus.devices, idev);

    bus_match();

    return 0;
}

struct input_device *
input_device_find(const char *name)
{
    for_each_in_list(struct input_device *, input_bus.devices, list, idev) {
	if (strcmp(idev->name, name) == 0) {
	    return idev;
	}
    }

    return NULL;
}
