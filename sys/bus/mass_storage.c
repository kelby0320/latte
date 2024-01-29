#include "bus/mass_storage.h"

#include "bus/ata/ata.h"

void
mass_storage_bus_init()
{
    ata_init();
}