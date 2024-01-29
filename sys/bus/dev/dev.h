#ifndef DEV_BUS_H
#define DEV_BUS_H

#include "bus/virtual.h"

struct dev_bus {
    struct virtual_bus virtual_bus;
};

int
dev_bus_init();

#endif