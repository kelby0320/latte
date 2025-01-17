#include "driver/platform/ata.h"

#include "port.h"
#include "device.h"
#include "driver.h"
#include "errno.h"
#include "libk/list.h"
#include "dev/platform_device.h"

static int
base_and_drive_from_resources(struct device *dev, unsigned int *base_addr, unsigned int *drive_no) {
    struct platform_device *pdev = as_platform_device(dev);

    struct resource *reg_base_resource = list_front(pdev->resources);
    if (!reg_base_resource) {
	return -EINVAL;
    }

    struct resource *drive_no_resource = list_front(pdev->resources->next);
    if (!drive_no_resource) {
	return -EINVAL;
    }

    *base_addr = reg_base_resource->base_addr;
    *drive_no  = drive_no_resource->base_addr;

    return 0;
}

int
ata_identify(struct device *dev, unsigned short *buf)
{
    unsigned int register_base;
    unsigned int drive_no;
    
    int res = base_and_drive_from_resources(dev, &register_base, &drive_no);
    if (res < 0) {
	return res;
    }
    
    unsigned int data_reg = register_base + ATA_PIO_DATA_REG_OFFSET;
    unsigned int sector_count_reg =
        register_base + ATA_PIO_SECTOR_COUNT_REG_OFFSET;
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
    for (int i = 0; i < 256; i++) {
        buf[i] = insw(data_reg);
    }

    return 0;
}

int
ata_read_sectors(struct device *dev, unsigned int lba, char *buf, size_t count)
{
    unsigned int register_base;
    unsigned int drive_no;
    
    int res = base_and_drive_from_resources(dev, &register_base, &drive_no);
    if (res < 0) {
	return res;
    }

    unsigned int data_reg = register_base + ATA_PIO_DATA_REG_OFFSET;
    unsigned int sector_count_reg =
        register_base + ATA_PIO_SECTOR_COUNT_REG_OFFSET;
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
    for (size_t i = 0; i < count; i++) {
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

int
ata_write_sectors(
    struct device *dev, unsigned int lba, const char *buf, size_t count)
{
    // TODO
    return 0;
}
