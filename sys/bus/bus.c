#include "bus/bus.h"

#include "bus/mass_storage.h"
#include "bus/virtual/virtual.h"
#include "config.h"
#include "errno.h"
#include "libk/print.h"

static struct bus *bus_list[LATTE_MAX_BUSES] = {0};
static int bus_list_len = 0;

void
bus_probe()
{
    for (int i = 0; i < bus_list_len; i++) {
        struct bus *bus = bus_list[i];
        int res = bus->probe(bus);
        if (res < 0) {
            printk("Failed to probe bus %s\n", bus);
        }
    }
}

void
bus_init()
{
    int res = mass_storage_bus_init();
    if (res < 0) {
        printk("Failed to initialize mass storage buses\n");
    }

    res = virtual_bus_init();
    if (res < 0) {
        printk("Failed to initialize virtual buses\n");
    }
}

int
bus_add_bus(struct bus *bus)
{
    if (bus_list_len == LATTE_MAX_BUSES) {
        return -EAGAIN;
    }

    bus_list[bus_list_len] = bus;
    bus_list_len++;
}