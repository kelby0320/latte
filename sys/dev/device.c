#include "dev/device.h"

#include "config.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "libk/string.h"

static struct device *device_list[LATTE_MAX_DEVICES] = {0};
static int device_list_len = 0;
static unsigned int next_device_id = 0;

int
device_get_next_device_id()
{
    return next_device_id++;
}

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

    return NULL;
}

int
device_iterator_init(struct device_iterator **iter_out)
{
    struct device_iterator *iter = kzalloc(sizeof(struct device_iterator));
    if (!iter) {
        return -ENOMEM;
    }

    iter->device_list = device_list;
    iter->device_list_len = &device_list_len;
    iter->current_idx = 0;

    *iter_out = iter;

    return 0;
}

void
device_iterator_free(struct device_iterator *iter)
{
    kfree(iter);
}

struct device *
device_iterator_val(struct device_iterator *iter)
{
    if (iter->current_idx >= *iter->device_list_len) {
        return NULL;
    }

    return iter->device_list[iter->current_idx];
}

int
device_iterator_next(struct device_iterator *iter)
{
    if (iter->current_idx >= *iter->device_list_len) {
        return -EINVAL;
    }

    iter->current_idx++;
    return iter->current_idx;
}