#ifndef VIRTUAL_BUS_H
#define VIRTUAL_BUS_H

#include "bus/bus.h"
#include "bus/mass_storage.h"

/**
 * @brief Virtual bus structure
 *
 */
struct virtual_bus {
    // Bus structure
    struct bus bus;
};

/**
 * @brief Virtual mass storage bus structure
 *
 */
struct virtual_mass_storage_bus {
    // Mass storage bus structure
    struct mass_storage_bus mass_storage_bus;
};

#define as_virtual_bus(ptr)              ((struct virtual_bus *)ptr)
#define as_virtual_mass_storage_bus(ptr) ((struct virtual_mass_storage_bus *)ptr)

/**
 * @brief Initialize virtual buses
 *
 * @return int
 */
int
virtual_bus_init();

struct virtual_bus *
virtual_bus_new();

struct virtual_mass_storage_bus *
virtual_mass_storage_bus_new();

#endif