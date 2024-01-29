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

/**
 * @brief Initialize virtual buses
 *
 * @return int
 */
int
virtual_bus_init();

#endif