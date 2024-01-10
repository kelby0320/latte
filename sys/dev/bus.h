#ifndef BUS_H
#define BUS_H

#include "config.h"

#include <stddef.h>

struct bus;

union bus_addr {
    void *ptr;
    unsigned int val;
};

struct io_operations {
    int (*probe)(struct bus *bus);
    int (*read)(struct bus *bus, unsigned int device_id, union bus_addr addr, char *buf,
                size_t count);
    int (*write)(struct bus *bus, unsigned int device_id, union bus_addr addr, const char *buf,
                 size_t count);
};

/**
 * @brief Bus interface structure
 *
 */
struct bus {
    // The name of the bus
    char name[LATTE_BUS_NAME_MAX_SIZE];

    // Bus Id
    unsigned int id;

    // Pointer to io_operations for the bus
    struct io_operations *io_operations;

    // Pointer to private data for the bus
    void *private;
};

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

/**
 * @brief Get a new bus id number
 *
 * @return int  Bus id
 */
int
bus_get_new_bus_id();

#endif