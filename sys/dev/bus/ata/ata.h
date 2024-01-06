#ifndef ATA_BUS_H
#define ATA_BUS_H

#include "dev/bus.h"

struct ata_bus_private {
    // Base address of ATA bus
    unsigned int base_addr;
};

int
ata_init();

#endif