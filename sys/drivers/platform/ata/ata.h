#ifndef DRIVERS_ATA_H
#define DRIVERS_ATA_H

#include <stddef.h>

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

struct device;
struct platform_device;

struct ata_private {
    unsigned int reg_base;
    unsigned int drive_no;
};

int
ata_drv_init();

int
ata_probe(struct platform_device *pdev);

int
ata_identify(struct device *dev, unsigned short *buf);

int
ata_read_sectors(struct device *dev, unsigned int lba, char *buf, size_t count);

int
ata_write_sectors(
    struct device *dev, unsigned int lba, const char *buf, size_t count);

#endif
