#ifndef BUS_H
#define BUS_H

#include "config.h"

#include <stddef.h>

/**
 * @brief Bus interface structure
 *
 */
struct bus {
    // The name of the bus
    char name[LATTE_BUS_NAME_MAX_SIZE];

    // Function to probe this bus for devices
    int (*probe)(struct bus *bus);
};

#define as_bus(ptr) ((struct bus *)ptr)

/**
 * @brief Probe system buses for devices
 *
 */
void
bus_probe();

/**
 * @brief Initialize the bus subsystem
 *
 */
void
bus_init();

/**
 * @brief Add a new bus to the system
 *
 * @param bus   Pointer to the bus
 * @return int  Status code
 */
int
bus_add_bus(struct bus *bus);

#endif