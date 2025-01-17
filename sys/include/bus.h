#ifndef DEV_BUS_H
#define DEV_BUS_H

struct device;
struct device_driver;
struct list_item;

/**
 * @brief Bus interface structure
 *
 */
struct bus {
    // Name of the bus
    const char *name;

    // Called to match a device to a device driver
    int (*match)(struct device *dev, struct device_driver *drv);

    // Called when a new device is added to this bus
    int (*probe)(struct device *dev);

    // List of devices on this bus
    struct list_item *devices;
};

int
bus_init();

int
bus_register(struct bus *bus);

int
bus_match();

int
bus_probe();

#endif
