#include "bus/dev/dev.h"

#include "bus/bus.h"
#include "dev/device.h"
#include "dev/term/term.h"
#include "dev/vga/vga.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/print.h"
#include "libk/string.h"

static unsigned int dev_bus_id = 0;

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
    struct dev_bus *dev_bus = kzalloc(sizeof(struct dev_bus));
    if (!dev_bus) {
        return -ENOMEM;
    }

    char name[LATTE_BUS_NAME_MAX_SIZE];
    sprintk(name, "dev%d", dev_bus_id);
    strcpy(dev_bus->virtual_bus.bus.name, name);
    dev_bus->virtual_bus.bus.probe = dev_bus_probe;
    dev_bus_id++;

    int res = bus_add_bus((struct bus *)dev_bus);
    if (res < 0) {
        kfree(dev_bus);
        return -EAGAIN;
    }

    printk("%s bus initialized successfully\n", name);

    return 0;
}