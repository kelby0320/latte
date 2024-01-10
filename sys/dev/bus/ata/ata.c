#include "dev/bus/ata/ata.h"
#include "dev/block/block.h"
#include "dev/bus.h"
#include "dev/device.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/string.h"
#include "port/io.h"

#define ATA_PIO_PRIMARY_BUS_BASE_ADDR   0x1F0
#define ATA_PIO_SECONDARY_BUS_BASE_ADDR 0x170

#define ATA_PIO_DRIVE_MASTER 0
#define ATA_PIO_DRIVE_SLAVE  1

#define ATA_PIO_DATA_REG_OFFSET         0
#define ATA_PIO_ERR_REG_OFFSET          1
#define ATA_PIO_FEATURE_REG_OFFSET      1
#define ATA_PIO_SECTOR_COUNT_REG_OFFSET 2
#define ATA_PIO_LBA_LOW_REG_OFFSET      3
#define ATA_PIO_LBA_MID_REG_OFFSET      4
#define ATA_PIO_LBA_HI_REG_OFFSET       5
#define ATA_PIO_DRIVE_REG_OFFSET        6
#define ATA_PIO_STATUS_REG_OFFSET       7
#define ATA_PIO_COMMAND_REG_OFFSET      7

#define ATA_PIO_CMD_READ_SECTORS 0x20
#define ATA_PIO_CMD_IDENTIFY     0xEC

#define ATA_PIO_STATUS_ERR  0b00000001
#define ATA_PIO_STATUS_IDX  0b00000010
#define ATA_PIO_STATUS_CORR 0b00000100
#define ATA_PIO_STATUS_DRQ  0b00001000
#define ATA_PIO_STATUS_SRV  0b00010000
#define ATA_PIO_STATUS_DF   0b00100000
#define ATA_PIO_STATUS_RDY  0b01000000
#define ATA_PIO_STATUS_BSY  0b10000000

#define lba_lo(lba) (unsigned char)(lba & 0xff)

#define lba_mid(lba) (unsigned char)(lba >> 8)

#define lba_hi(lba) (unsigned char)(lba >> 16)

#define lba_drive(lba, drive_no) (lba >> 24) | 0b11100000 | (drive_no << 4)

static int
ata_bus_identify(struct bus *bus, int drive_no)
{
    struct ata_bus_private *ata_private = (struct ata_bus_private *)bus->private;

    unsigned int register_base = ata_private->base_addr;

    unsigned int data_reg = register_base + ATA_PIO_DATA_REG_OFFSET;
    unsigned int sector_count_reg = register_base + ATA_PIO_SECTOR_COUNT_REG_OFFSET;
    unsigned int lba_lo_reg = register_base + ATA_PIO_LBA_LOW_REG_OFFSET;
    unsigned int lba_mid_reg = register_base + ATA_PIO_LBA_MID_REG_OFFSET;
    unsigned int lba_hi_reg = register_base + ATA_PIO_LBA_HI_REG_OFFSET;
    unsigned int drive_reg = register_base + ATA_PIO_DRIVE_REG_OFFSET;
    unsigned int cmd_reg = register_base + ATA_PIO_COMMAND_REG_OFFSET;
    unsigned int status_reg = register_base + ATA_PIO_STATUS_REG_OFFSET;

    outb(drive_reg, 0xA0 | (drive_no << 4));
    outb(sector_count_reg, 0);
    outb(lba_lo_reg, 0);
    outb(lba_mid_reg, 0);
    outb(lba_hi_reg, 0);
    outb(cmd_reg, ATA_PIO_CMD_IDENTIFY);

    char buf[512];
    for (int b = 0; b < 512; b++) {
        char c = insb(status_reg);
        if (c == 0) {
            // The drive does not exist
            return -EEXIST;
        }

        // Wait for busy to clear
        while (!(c & ATA_PIO_STATUS_BSY)) {
            c = insb(status_reg);
        }

        c = insb(lba_mid_reg);
        if (c != 0) {
            // Not an ATA drive
            return -EEXIST;
        }

        c = insb(lba_hi_reg);
        if (c != 0) {
            // Not an ATA drive
            return -EEXIST;
        }

        // Wait for the buffer to be ready
        while (!(c & ATA_PIO_STATUS_DRQ)) {
            c = insb(status_reg);
        }

        // Copy from identify structure to memory
        char *ptr = buf;
        for (int i = 0; i < 256; i++) {
            *ptr = insw(data_reg);
            ptr++;
        }
    }
}

static int
add_block_device(struct bus *bus, int drive_no)
{
    struct device *device = kzalloc(sizeof(struct device));
    if (!device) {
        return -ENOMEM;
    }

    device->bus = bus;
    device->id = drive_no;

    int res = block_device_init(device);
    if (res < 0) {
        goto err_out;
    }

    res = device_add_device(device);
    if (res < 0) {
        goto err_out;
    }

    return 0;

err_out:
    kfree(device);
    return res;
}

static int
ata_probe_drive(struct bus *bus, int drive_no)
{
    int res = ata_bus_identify(bus, drive_no);
    if (res) {
        return add_block_device(bus, drive_no);
    }

    return 0;
}

static int
ata_bus_probe(struct bus *bus)
{
    int res = ata_probe_drive(bus, ATA_PIO_DRIVE_MASTER);
    if (res < 0) {
        return res;
    }

    res = ata_probe_drive(bus, ATA_PIO_DRIVE_SLAVE);
    if (res < 0) {
        return res;
    }

    return 0;
}

static int
ata_bus_read(struct bus *bus, unsigned int device_id, union bus_addr addr, char *buf,
             size_t sector_count)
{
    struct ata_bus_private *ata_private = (struct ata_bus_private *)bus->private;

    unsigned int register_base = ata_private->base_addr;
    unsigned int drive_no = device_id;
    unsigned int lba = addr.val;

    // drive_no must be either 0 or 1
    if (drive_no > 2) {
        return -EINVAL;
    }

    unsigned int data_reg = register_base + ATA_PIO_DATA_REG_OFFSET;
    unsigned int sector_count_reg = register_base + ATA_PIO_SECTOR_COUNT_REG_OFFSET;
    unsigned int lba_lo_reg = register_base + ATA_PIO_LBA_LOW_REG_OFFSET;
    unsigned int lba_mid_reg = register_base + ATA_PIO_LBA_MID_REG_OFFSET;
    unsigned int lba_hi_reg = register_base + ATA_PIO_LBA_HI_REG_OFFSET;
    unsigned int drive_reg = register_base + ATA_PIO_DRIVE_REG_OFFSET;
    unsigned int cmd_reg = register_base + ATA_PIO_COMMAND_REG_OFFSET;
    unsigned int status_reg = register_base + ATA_PIO_STATUS_REG_OFFSET;

    outb(drive_reg, lba_drive(lba, drive_no));
    outb(sector_count_reg, sector_count);
    outb(lba_lo_reg, lba_lo(lba));
    outb(lba_mid_reg, lba_mid(lba));
    outb(lba_hi_reg, lba_hi(lba));
    outb(cmd_reg, ATA_PIO_CMD_READ_SECTORS);

    unsigned short *ptr = (unsigned short *)buf;
    for (size_t i = 0; i < sector_count; i++) {
        // Wait for the buffer to be ready
        char c = insb(status_reg);
        while (!(c & ATA_PIO_STATUS_DRQ)) {
            c = insb(status_reg);
        }

        // Copy from hard disk to memory
        for (int j = 0; j < 256; j++) {
            *ptr = insw(data_reg);
            ptr++;
        }
    }

    return 0;
}

static int
ata_bus_write(struct bus *bus, unsigned int device_id, union bus_addr addr, const char *buf,
              size_t sector_count)
{
    // TODO
    return 0;
}

int
ata_init_bus(unsigned int base_addr, const char *name, unsigned int id)
{
    struct bus *ata_bus = kzalloc(sizeof(struct bus));
    if (!ata_bus) {
        return -ENOMEM;
    }

    struct ata_bus_private *ata_bus_private = kzalloc(sizeof(struct ata_bus_private));
    if (!ata_bus_private) {
        return -ENOMEM;
    }

    ata_bus_private->base_addr = base_addr;

    strcpy(ata_bus->name, name);
    ata_bus->id = id;

    ata_bus->io_operations->probe = ata_bus_probe;
    ata_bus->io_operations->read = ata_bus_read;
    ata_bus->io_operations->write = ata_bus_write;

    int res = bus_add_bus(ata_bus);
    if (!res) {
        kfree(ata_bus_private);
        kfree(ata_bus);
        return -EAGAIN;
    }

    return 0;
}

int
ata_init()
{
    unsigned int bus_id = bus_get_new_bus_id();
    int res = ata_init_bus(ATA_PIO_PRIMARY_BUS_BASE_ADDR, "ata0", bus_id);
    if (res < 0) {
        goto err_out;
    }

    bus_id = bus_get_new_bus_id();
    res = ata_init_bus(ATA_PIO_SECONDARY_BUS_BASE_ADDR, "ata1", bus_id);
    if (res < 0) {
        goto err_out;
    }

err_out:
    return res;
}
