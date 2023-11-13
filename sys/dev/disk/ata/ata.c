#include "dev/disk/ata/ata.h"

#include "config.h"
#include "dev/disk/disk.h"
#include "errno.h"
#include "fs/fs.h"
#include "port/io.h"
#include "libk/kheap.h"
#include "libk/string.h"

int
ata_read_sectors(struct disk *disk, unsigned int lba, int total, void *buf)
{
    unsigned int register_base = ((struct ata*)disk->private)->bus_base_addr;
    unsigned int drive_no = ((struct ata*)disk->private)->drive_no;

    unsigned int data_reg = register_base + ATA_PIO_DATA_REG_OFFSET;
    unsigned int sector_count_reg = register_base + ATA_PIO_SECTOR_COUNT_REG_OFFSET;
    unsigned int lba_lo_reg = register_base + ATA_PIO_LBA_LOW_REG_OFFSET;
    unsigned int lba_mid_reg = register_base + ATA_PIO_LBA_MID_REG_OFFSET;
    unsigned int lba_hi_reg = register_base + ATA_PIO_LBA_HI_REG_OFFSET;
    unsigned int drive_reg = register_base + ATA_PIO_DRIVE_REG_OFFSET;
    unsigned int cmd_reg = register_base + ATA_PIO_COMMAND_REG_OFFSET;
    unsigned int status_reg = register_base + ATA_PIO_STATUS_REG_OFFSET;

    outb(drive_reg, lba_drive(lba, drive_no));
    outb(sector_count_reg, total);
    outb(lba_lo_reg, lba_lo(lba));
    outb(lba_mid_reg, lba_mid(lba));
    outb(lba_hi_reg, lba_hi(lba));
    outb(cmd_reg, ATA_PIO_CMD_READ_SECTORS);

    unsigned short* ptr = (unsigned short*) buf;
    for (int b = 0; b < total; b++) {
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

int
ata_probe_and_init()
{
    struct disk *disk = disk_get_free_disk();
    if (!disk) {
        return -EIO;
    }

    struct ata *ata = kmalloc(sizeof(struct ata));
    ata->bus_base_addr = ATA_PIO_PRIMARY_BUS_BASE_ADDR;
    ata->drive_no = ATA_PIO_DRIVE_MASTER;

    disk->type = DISK_TYPE_ATA;
    strncpy(disk->id, "hdd0", LATTE_DISK_ID_MAX_SIZE);
    disk->private = ata;
    disk->read_sectors = ata_read_sectors;

    disk->fs = fs_resolve(disk);

    return 0;
}