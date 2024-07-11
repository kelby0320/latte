#include "drivers/driver.h"

#include "dev/bus.h"
#include "drivers/platform/ata/ata.h"
#include "drivers/platform/devfs/devfs.h"
#include "drivers/platform/platform_driver.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "libk/string.h"

#include <stddef.h>

static struct list_item *driver_list = NULL;

static int
add_platform_drivers()
{
    ata_drv_init();
    devfs_drv_init();
    
    return 0;
}

int
driver_init()
{
    add_platform_drivers();
    
    return 0;
}

int
driver_register(struct device_driver *driver)
{
    list_push_front(&driver_list, driver);

    bus_match();
    bus_probe();

    return 0;
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
	return list_front(driver_list);
    }

    for (struct list_item *p = driver_list; p; p = p->next) {
	if (p->data == driver) {
	    return list_next(p);
	}
    }

    return NULL;
}
