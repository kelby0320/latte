#include "dev/device.h"

#include "config.h"
#include "errno.h"

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