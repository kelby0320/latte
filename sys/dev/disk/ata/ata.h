#ifndef ATA_H
#define ATA_H

#define ATA_SECTOR_SIZE 512

#define ATA_PIO_PRIMARY_BUS_BASE_ADDR 0x1F0
#define ATA_PIO_SECONDARY_BUS_BASE_ADDR 0x170

#define ATA_PIO_DRIVE_MASTER 0
#define ATA_PIO_DRIVE_SLAVE 1

#define ATA_PIO_DATA_REG_OFFSET 0
#define ATA_PIO_ERR_REG_OFFSET 1
#define ATA_PIO_FEATURE_REG_OFFSET 1
#define ATA_PIO_SECTOR_COUNT_REG_OFFSET 2
#define ATA_PIO_LBA_LOW_REG_OFFSET 3
#define ATA_PIO_LBA_MID_REG_OFFSET 4
#define ATA_PIO_LBA_HI_REG_OFFSET 5
#define ATA_PIO_DRIVE_REG_OFFSET 6
#define ATA_PIO_STATUS_REG_OFFSET 7
#define ATA_PIO_COMMAND_REG_OFFSET 7

#define ATA_PIO_CMD_READ_SECTORS 0x20

#define ATA_PIO_STATUS_ERR  0b00000001
#define ATA_PIO_STATUS_IDX  0b00000010
#define ATA_PIO_STATUS_CORR 0b00000100
#define ATA_PIO_STATUS_DRQ  0b00001000
#define ATA_PIO_STATUS_SRV  0b00010000
#define ATA_PIO_STATUS_DF   0b00100000
#define ATA_PIO_STATUS_RDY  0b01000000
#define ATA_PIO_STATUS_BSY  0b10000000

#define lba_lo(lba)                 \
    (unsigned char)(lba & 0xff)     \

#define lba_mid(lba)                \
    (unsigned char)(lba >> 8)       \

#define lba_hi(lba)                 \
    (unsigned char)(lba >> 16)      \

#define lba_drive(lba, drive_no)    \
    (lba >> 24) | 0b00000111 | (drive_no << 3)  \


struct ata {
    unsigned int bus_base_addr;
    unsigned int drive_no;
};

int
ata_probe_and_init();

#endif