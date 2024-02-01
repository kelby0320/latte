#ifndef MASS_STORAGE_H
#define MASS_STORAGE_H

#include "bus/bus.h"

#include <stdint.h>

/**
 * @brief Mass storage bus interface
 *
 */
struct mass_storage_bus {
    // Bus structure
    struct bus bus;

    // Function to read from the bus
    int (*read)(struct mass_storage_bus *mass_storage_bus, unsigned int selector, uint64_t saddr,
                char *buf, size_t count);

    // Function to write to the bus
    int (*write)(struct mass_storage_bus *mass_storage_bus, unsigned int selector, uint64_t saddr,
                 const char *buf, size_t count);
};

#define as_mass_storage_bus(ptr) ((struct mass_storage_bus *)ptr)

int
mass_storage_bus_init();

#endif