#ifndef MASS_STORAGE_H
#define MASS_STORAGE_H

#include "bus/bus.h"

#include <stdint.h>

struct mass_storage_bus {
    struct bus bus;

    // Function to read from the bus
    int (*read)(struct mass_storage_bus *mass_storage_bus, unsigned int selector, uint64_t saddr,
                char *buf, size_t count);

    // Function to write to the bus
    int (*write)(struct mass_storage_bus *mass_storage_bus, unsigned int selector, uint64_t saddr,
                 const char *buf, size_t count);
};

void
mass_storage_bus_init();

#endif