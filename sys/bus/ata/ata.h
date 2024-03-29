#ifndef ATA_BUS_H
#define ATA_BUS_H

#include "bus/mass_storage.h"

/**
 * @brief ATA bus structure
 *
 */
struct ata_bus {
    // Mass storage bus structure
    struct mass_storage_bus mass_storage_bus;

    // Base address of ATA bus
    unsigned int base_addr;
};

#define as_ata_bus(ptr) ((struct ata_bus *)ptr)

/**
 * @brief Initialize all ATA buses
 *
 * @return int  Status code
 */
int
ata_bus_init();

#endif