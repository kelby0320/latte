#include "dev/input_device.h"

#include "bus.h"
#include "driver.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/memory.h"
#include "libk/print.h"
#include "libk/string.h"
#include "libk/list.h"

#include <stdbool.h>

struct list_item *input_device_list = NULL;

int
input_device_init(struct input_device *idev, const char *name, input_device_type_t type)
{
    if (type > INPUT_DEVICE_TYPE_MOUSE) {
	return -EINVAL;
    }

    idev->type = type;
    
    int name_len = strlen(name);
    idev->name = kzalloc(name_len + 1);
    if (!idev->name) {
	return -ENOMEM;
    }

    strcpy((char *)idev->name, name);

    idev->input_recv_callbacks = NULL;

    return 0;

err_kbd:
    kfree((void *)idev->name);
    return -ENOMEM;
}

int
input_device_add_callback(struct input_device *idev, input_recv_callback_t callback)
{
    return list_push_back(&idev->input_recv_callbacks, callback);
}

int
input_device_register(struct input_device *idev)
{
    return list_push_front(&input_device_list, idev);
}

struct input_device *
input_device_find(const char *name)
{
    for_each_in_list(struct input_device *, input_device_list, list, idev) {
	if (strcmp(idev->name, name) == 0) {
	    return idev;
	}
    }

    return NULL;
}

int
input_device_event(struct input_device *idev, struct input_event event)
{
    for_each_in_list(input_recv_callback_t, idev->input_recv_callbacks, list, callback) {
	callback(idev, event);
    }

    return 0;
}
