#include "bus/ata/ata.h"

#include "bus/bus.h"
#include "bus/mass_storage.h"
#include "dev/block/block.h"
#include "dev/device.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "libk/print.h"
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

static unsigned int ata_bus_id = 0;

/**
 * @brief Identify a drive on an ATA bus
 *
 * @param ata_bus   Pointer to the ATA bus
 * @param drive_no  ATA drive number
 * @return int      Status code
 */
static int
ata_bus_identify(struct ata_bus *ata_bus, int drive_no)
{
    unsigned int register_base = ata_bus->base_addr;

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

    char c = insb(status_reg);
    if (c == 0) {
        // The drive does not exist
        return -EEXIST;
    }

    // Wait for busy to clear
    while (c & ATA_PIO_STATUS_BSY) {
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
    unsigned short buf[256];
    for (int i = 0; i < 256; i++) {
        buf[i] = insw(data_reg);
    }

    return 0;
}

/**
 * @brief Add a block device to the system
 *
 * @param ata_bus           Pointer to the bus
 * @param type              Type of block device
 * @param drive_no          Drive number of the block device
 * @param lba_offset        LBA offset of the block device
 * @param block_device_out  Pointer to new block device
 * @return int              Status code
 */
static int
add_block_device(struct ata_bus *ata_bus, block_device_type_t type, unsigned int drive_no,
                 unsigned int lba_offset, struct block_device **block_device_out)
{
    struct device *device = kzalloc(sizeof(struct device));
    if (!device) {
        return -ENOMEM;
    }

    device->type = DEVICE_TYPE_BLOCK;
    device->bus = (struct bus *)ata_bus;

    int res = block_device_init((struct block_device *)device, type, drive_no, lba_offset);
    if (res < 0) {
        goto err_out;
    }

    res = device_add_device(device);
    if (res < 0) {
        goto err_out;
    }

    *block_device_out = (struct block_device *)device;

    return 0;

err_out:
    kfree(device);
    return res;
}

/**
 * @brief Add a block device for each partition on a disk block device
 *
 * @param block_device_disk Pointer to block device of type disk
 * @return int              Status code
 */
static int
add_block_device_partitions(struct block_device *block_device_disk)
{
    struct partition_table_entry partition_table[4];
    int res = block_device_read_partitions(block_device_disk, partition_table);
    if (res < 0) {
        return res;
    }

    struct ata_bus *ata_bus = (struct ata_bus *)block_device_disk->device.bus;
    unsigned int drive_no = block_device_disk->drive_no;

    for (int i = 0; i < 4; i++) {
        if (partition_table[i].sector_count == 0) {
            continue; // Table entry doesn't correspond to a partition
        }

        unsigned int lba_offset = partition_table[i].lba_start;
        struct block_device *block_device_part;

        printk("Found partition starting at %d on %s\n", lba_offset,
               block_device_disk->device.name);

        res = add_block_device(ata_bus, BLOCK_DEVICE_TYPE_PART, drive_no, lba_offset,
                               &block_device_part);
        if (res < 0) {
            return res;
        }

        printk("Added block device %s of type %d\n", block_device_part->device.name,
               BLOCK_DEVICE_TYPE_PART);
    }

    return 0;
}

/**
 * @brief Add a block device to the bus
 *
 * @param ata_bus   Pointer to the ATA bus
 * @param drive_no  ATA drive number
 * @return int      Status code
 */
static int
add_block_devices(struct ata_bus *ata_bus, int drive_no)
{
    struct block_device *block_device_disk;
    int res = add_block_device(ata_bus, BLOCK_DEVICE_TYPE_DISK, drive_no, 0, &block_device_disk);
    if (res < 0) {
        return res;
    }

    printk("Added block device %s of type %d\n", block_device_disk->device.name,
           BLOCK_DEVICE_TYPE_DISK);

    res = add_block_device_partitions(block_device_disk);
    if (res < 0) {
        return res;
    }

    return 0;
}

/**
 * @brief Probe an ATA bus
 *
 * @param bus       Pointer to the bus
 * @param drive_no  ATA drive number
 * @return int      Status code
 */
static int
ata_probe_drive(struct bus *bus, int drive_no)
{
    struct ata_bus *ata_bus = (struct ata_bus *)bus;

    int res = ata_bus_identify(ata_bus, drive_no);
    if (res == 0) {
        printk("Found hard drive on %s\n", ata_bus->mass_storage_bus.bus.name);

        return add_block_devices(ata_bus, drive_no);
    }

    return 0;
}

/**
 * @brief       Probe for ATA buses
 *
 * @param bus   Pointer to the bus
 * @return int  Status code
 */
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

/**
 * @brief Read from an ATA bus
 *
 * @param mass_storage_bus  Pointer to the mass storage bus
 * @param drive_no          Drive number
 * @param lba               LBA to read from
 * @param buf               Buffer to read into
 * @param sector_count      Number of sectores to read
 * @return int              Status code
 */
static int
ata_bus_read(struct mass_storage_bus *mass_storage_bus, unsigned int selector, uint64_t saddr,
             char *buf, size_t sector_count)
{
    struct ata_bus *ata_bus = (struct ata_bus *)mass_storage_bus;

    unsigned int register_base = ata_bus->base_addr;
    unsigned int drive_no = selector;
    unsigned int lba = saddr;

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

/**
 * @brief Write to an ATA bus
 *
 * @param mass_storage_bus  Pointer to the mass storage bus
 * @param drive_no
 * @param lba               LBA to write to
 * @param buf               Buffer to write
 * @param size              Size of the buffer
 * @return int              Status code
 */
static int
ata_bus_write(struct mass_storage_bus *mass_storage_bus, unsigned int selector, uint64_t saddr,
              const char *buf, size_t size)
{
    // TODO
    return 0;
}

/**
 * @brief Initialize an ATA bus
 * @param base_addr     Base address of the ATA registers
 * @param name          Name of the bus
 * @param id            Bus Id
 * @return int          Status code
 */
int
ata_init_bus(unsigned int base_addr, const char *name, unsigned int id)
{
    struct ata_bus *ata_bus = kzalloc(sizeof(struct ata_bus));
    if (!ata_bus) {
        return -ENOMEM;
    }

    ata_bus->base_addr = base_addr;

    strcpy(ata_bus->mass_storage_bus.bus.name, name);

    ata_bus->mass_storage_bus.bus.probe = ata_bus_probe;
    ata_bus->mass_storage_bus.read = ata_bus_read;
    ata_bus->mass_storage_bus.write = ata_bus_write;

    int res = bus_add_bus((struct bus *)ata_bus);
    if (!res) {
        kfree(ata_bus);
        return -EAGAIN;
    }

    return 0;
}

int
ata_bus_init()
{
    char name[LATTE_BUS_NAME_MAX_SIZE];
    sprintk(name, "ata%d", ata_bus_id);
    int res = ata_init_bus(ATA_PIO_PRIMARY_BUS_BASE_ADDR, name, ata_bus_id);
    ata_bus_id++;
    if (res < 0) {
        return res;
    }

    printk("%s bus initialized successfully\n", name);

    sprintk(name, "ata%d", ata_bus_id);
    res = ata_init_bus(ATA_PIO_SECONDARY_BUS_BASE_ADDR, name, ata_bus_id);
    if (res < 0) {
        return res;
    }

    printk("%s bus initialized successfully\n", name);

    return 0;
}
