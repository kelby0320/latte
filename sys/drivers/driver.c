#include "drivers/driver.h"

#include "libk/list.h"
#include "libk/string.h"
#include "dev/bus.h"

#include <stddef.h>

static struct list_item *driver_list = NULL;

int
driver_init()
{
    /* TODO - register drivers */
    return 0;
}

int
driver_register(struct device_driver *driver)
{
    list_push_front(&driver_list, driver);

    bus_match();
    bus_probe();
}

struct device_driver *
driver_find(const char *name)
{
    for_each_in_list(struct device_driver *, driver_list, list, driver)
    {
        if (strcmp(driver->name, name) == 0) {
            return driver;
        }
    }

    return NULL;
}

struct device_driver *
driver_find_next(const struct device_driver *driver)
{
    if (driver == NULL) {
        return driver_list->data;
    }

    struct list_item *item = list_container_of(driver);
    return item->next->data;
}
