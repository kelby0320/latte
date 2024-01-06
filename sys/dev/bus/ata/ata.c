#include "dev/bus/ata/ata.h"
#include "dev/bus.h"
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
ata_bus_probe(void *private)
{
    // TODO
    return 0;
}

static int
ata_bus_read(void *private, unsigned int device_id, union bus_addr addr, char *buf, size_t count)
{
    struct ata_bus_private *ata_private = (struct ata_bus_private *)private;

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
    outb(sector_count_reg, count);
    outb(lba_lo_reg, lba_lo(lba));
    outb(lba_mid_reg, lba_mid(lba));
    outb(lba_hi_reg, lba_hi(lba));
    outb(cmd_reg, ATA_PIO_CMD_READ_SECTORS);

    unsigned short *ptr = (unsigned short *)buf;
    for (int b = 0; b < count; b++) {
        // Wait for the buffer to be ready
        char c = insb(status_reg);
        while (!(c & ATA_PIO_STATUS_DRQ)) {
            c = insb(status_reg);
        }

        // Copy from hard disk to memory
        for (int i = 0; i < 256; i++) {
            *ptr = insw(data_reg);
            ptr++;
        }
    }

    return 0;
}

static int
ata_bus_write(void *private, unsigned int device_id, union bus_addr addr, const char *buf,
              size_t count)
{
    // TODO
    return 0;
}

int
ata_init_bus(unsigned int base_addr, const char *name)
{
    struct bus *ata_primary = kzalloc(sizeof(struct bus));
    if (!ata_primary) {
        return -ENOMEM;
    }

    struct ata_bus_private *ata_primary_private = kzalloc(sizeof(struct ata_bus_private));
    if (!ata_primary_private) {
        return -ENOMEM;
    }

    ata_primary_private->base_addr = base_addr;

    strcpy(ata_primary->name, name);

    ata_primary->io_operations->probe = ata_bus_probe;
    ata_primary->io_operations->read = ata_bus_read;
    ata_primary->io_operations->write = ata_bus_write;

    int res = bus_add_bus(ata_primary);
    if (!res) {
        kfree(ata_primary_private);
        kfree(ata_primary);
        return -EAGAIN;
    }

    return 0;
}

int
ata_init()
{
    int res = ata_init_bus(ATA_PIO_PRIMARY_BUS_BASE_ADDR, "ata0");
    if (res < 0) {
        goto err_out;
    }

    res = ata_init_bus(ATA_PIO_SECONDARY_BUS_BASE_ADDR, "ata1");
    if (res < 0) {
        goto err_out;
    }

err_out:
    return res;
}
