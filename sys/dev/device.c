#include "device.h"

#include "errno.h"
#include "libk/alloc.h"
#include "libk/string.h"

static unsigned int next_id = 0;

int
make_device(struct device *dev, const char *name)
{
    int name_len = strlen(name);
    dev->name = kzalloc(name_len + 1);
    if (!dev->name) {
        return -ENOMEM;
    }

    strncpy(dev->name, name, name_len);
    dev->id = next_id++;

    return 0;
}
