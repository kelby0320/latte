#ifndef ATA_BUS_H
#define ATA_BUS_H

#include "bus/bus.h"

/**
 * @brief ATA bus structure
 * 
 */
struct ata_bus {
    // Bus structure
    struct bus bus;

    // Base address of ATA bus
    unsigned int base_addr;

    // Function to read from the bus
    int (*read)(struct ata_bus *ata_bus, unsigned int drive_no, unsigned int lba, char *buf, size_t count);

    // Function to write to the bus
    int (*write)(struct ata_bus *ata_bus, unsigned int drive_no, unsigned int lba, const char *buf, size_t count);
};

/**
 * @brief Initialize all ATA buses
 * 
 * @return int  Status code
 */
int
ata_init();

#endif