#include "bus/virtual/dev/dev.h"

#include "bus/bus.h"
#include "bus/virtual/virtual.h"
#include "dev/device.h"
#include "dev/term/term.h"
#include "dev/vga/vga.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/print.h"
#include "libk/string.h"

static int
add_term_device(struct bus *bus)
{
    struct term_device *term_device = kzalloc(sizeof(struct term_device));
    if (!term_device) {
        return -ENOMEM;
    }

    term_device->device.bus = bus;
    term_device->device.type = DEVICE_TYPE_TERM;

    int res = term_device_init(term_device);
    if (res < 0) {
        return res;
    }

    return device_add_device((struct device *)term_device);
}

static int
add_vga_device(struct bus *bus)
{
    struct vga_device *vga_device = kzalloc(sizeof(struct vga_device));
    if (!vga_device) {
        return -ENOMEM;
    }

    vga_device->device.bus = bus;
    vga_device->device.type = DEVICE_TYPE_VGA;

    int res = vga_device_init(vga_device);
    if (res < 0) {
        return res;
    }

    return device_add_device((struct device *)vga_device);
}

static int
dev_bus_probe(struct bus *bus)
{
    int res = add_vga_device(bus);
    if (res < 0) {
        return res;
    }

    res = add_term_device(bus);
    if (res < 0) {
        return res;
    }

    return 0;
}

int
dev_bus_init()
{
    struct virtual_bus *dev_bus = virtual_bus_new();
    if (!dev_bus) {
        return -ENOMEM;
    }

    dev_bus->bus.probe = dev_bus_probe;

    int res = bus_add_bus(as_bus(dev_bus));
    if (res < 0) {
        kfree(dev_bus);
        return -EAGAIN;
    }

    printk("%s bus initialized successfully\n", dev_bus->bus.name);

    return 0;
}