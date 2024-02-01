#include "bus/virtual/devfs/devfs.h"

#include "bus/bus.h"
#include "bus/virtual/virtual.h"
#include "dev/block/block.h"
#include "dev/device.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/print.h"
#include "libk/string.h"

/**
 * @brief Probe the devfs virtual bus
 *
 * @param bus   Pointer to the bus
 * @return int  Status code
 */
static int
devfs_bus_probe(struct bus *bus)
{
    struct device *device = kzalloc(sizeof(struct device));
    if (!device) {
        return -ENOMEM;
    }

    device->type = DEVICE_TYPE_BLOCK;
    device->bus = bus;

    int res = block_device_init((struct block_device *)device, BLOCK_DEVICE_TYPE_VIRT, 0, 0);
    if (res < 0) {
        goto err_out;
    }

    res = device_add_device(device);
    if (res < 0) {
        goto err_out;
    }

    printk("Added block device %s of type %d\n", device->name, BLOCK_DEVICE_TYPE_VIRT);

    return 0;

err_out:
    kfree(device);
    return res;
}

/**
 * @brief Read from the devfs virtual bus
 *
 * Since this is a virtual bus there is nothing to read from,
 * but this function will return the name of the bus on reads.
 *
 * @param mass_storage_bus  Pointer to mass storage bus
 * @param selector          Device selector
 * @param saddr             Start address
 * @param buf               Buf to read into
 * @param sector_count      Number of sectors to read
 * @return int              Status code
 */
static int
devfs_bus_read(struct mass_storage_bus *mass_storage_bus, unsigned int selector, uint64_t saddr,
               char *buf, size_t sector_count)
{
    sprintk(buf, "devfs");
    return 5;
}

/**
 * @brief Write to the devfs virtual bus
 *
 * Since this is a virtual bus there is nothing to write to
 *
 * @param mass_storage_bus  Pointer to mass storage bus
 * @param selector          Device selector
 * @param saddr             Start address
 * @param buf               Buffer to write
 * @param size              Number of bytes to write
 * @return int              -EIO
 */
static int
devfs_bus_write(struct mass_storage_bus *mass_storage_bus, unsigned int selector, uint64_t saddr,
                const char *buf, size_t size)
{
    // Writes are not supported
    return -EIO;
}

int
devfs_bus_init()
{
    struct virtual_mass_storage_bus *devfs_bus = virtual_mass_storage_bus_new();
    if (!devfs_bus) {
        return -ENOMEM;
    }

    devfs_bus->mass_storage_bus.bus.probe = devfs_bus_probe;
    devfs_bus->mass_storage_bus.read = devfs_bus_read;
    devfs_bus->mass_storage_bus.write = devfs_bus_write;

    int res = bus_add_bus(as_bus(devfs_bus));
    if (!res) {
        kfree(devfs_bus);
        return -EAGAIN;
    }

    printk("%s bus initialized successfully\n", devfs_bus->mass_storage_bus.bus.name);

    return 0;
}
