#include "bus/bus.h"

#include "bus/mass_storage.h"
#include "config.h"
#include "errno.h"

static struct bus *bus_list[LATTE_MAX_BUSES] = {0};
static int bus_list_len = 0;
static unsigned int next_bus_id = 0;

void
bus_probe()
{
    for (int i = 0; i < bus_list_len; i++) {
        bus_list[i]->probe(bus_list[i]);
    }
}

void
bus_init()
{
    mass_storage_bus_init();
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

int
bus_get_next_bus_id()
{
    return next_bus_id++;
}