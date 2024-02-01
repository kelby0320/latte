#include "bus/virtual/virtual.h"

#include "bus/virtual/dev/dev.h"
#include "bus/virtual/devfs/devfs.h"
#include "libk/kheap.h"
#include "libk/print.h"
#include "libk/string.h"

static unsigned int virtual_bus_id = 0;
static unsigned int virtual_mass_storage_bus_id = 0;

static void
base_bus_init(struct bus *bus, const char *bus_name, unsigned int bus_id)
{
    char name[LATTE_BUS_NAME_MAX_SIZE];
    sprintk(name, "%s%d", bus_name, bus_id);
    strcpy(bus->name, name);
}

int
virtual_bus_init()
{
    int res = devfs_bus_init();
    if (res < 0) {
        printk("Failed to initialize devfs bus\n");
    }

    res = dev_bus_init();
    if (res < 0) {
        printk("Failed to initialize dev bus\n");
    }

    return 0;
}

struct virtual_bus *
virtual_bus_new()
{
    struct virtual_bus *virtual_bus = kzalloc(sizeof(struct virtual_bus));
    if (!virtual_bus) {
        return NULL;
    }

    base_bus_init(&virtual_bus->bus, "virt", virtual_bus_id);
    virtual_bus_id++;

    return virtual_bus;
}

struct virtual_mass_storage_bus *
virtual_mass_storage_bus_new()
{
    struct virtual_mass_storage_bus *virtual_mass_storage_bus =
        kzalloc(sizeof(struct virtual_mass_storage_bus));
    if (!virtual_mass_storage_bus) {
        return NULL;
    }

    base_bus_init(&virtual_mass_storage_bus->mass_storage_bus.bus, "virtms",
                  virtual_mass_storage_bus_id);
    virtual_mass_storage_bus_id++;

    return virtual_mass_storage_bus;
}