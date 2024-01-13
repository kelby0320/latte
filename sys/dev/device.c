#include "dev/device.h"

#include "config.h"
#include "errno.h"
#include "libk/string.h"

static struct device *device_list[LATTE_MAX_DEVICES];
static int device_list_len;

int
device_add_device(struct device *device)
{
    if (device_list_len == LATTE_MAX_BUSES) {
        return -EAGAIN;
    }

    device_list[device_list_len] = device;
    device_list_len++;
}

struct device *
device_find(const char *name)
{
    for (int i = 0; i < device_list_len; i++) {
        struct device *device = device_list[i];
        if (strcmp(device->name, name) == 0) {
            return device;
        }
    }

    return 0;
}