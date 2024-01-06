#include "dev/bus.h"

#include "config.h"
#include "dev/bus/ata/ata.h"
#include "errno.h"
#include "libk/string.h"

static struct bus *bus_list[LATTE_MAX_BUSES];
static int bus_list_len;

void
bus_probe()
{
    for (int i = 0; i < bus_list_len; i++) {
        void *private = bus_list[i]->private;
        bus_list[i]->io_operations->probe(private);
    }
}

void
bus_init()
{
    ata_init();
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