#include "dev/device.h"

#include "config.h"
#include "dev/disk/disk.h"

static struct device *device_list[LATTE_MAX_DEVICES];
static int device_list_len;