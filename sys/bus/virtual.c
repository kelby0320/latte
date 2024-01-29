#include "bus/virtual.h"

#include "bus/devfs/devfs.h"

int
virtual_bus_init()
{
    return devfs_bus_init();
}