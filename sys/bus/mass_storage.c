#include "bus/mass_storage.h"

#include "bus/ata/ata.h"

int
mass_storage_bus_init()
{
    return ata_bus_init();
}