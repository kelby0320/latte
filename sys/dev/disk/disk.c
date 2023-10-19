#include "dev/disk/disk.h"

#include "config.h"
#include "dev/disk/ata/ata.h"
#include "errno.h"
#include "kernel.h"
#include "libk/kheap.h"
#include "libk/memory.h"

struct disk* disks[LATTE_MAX_DISKS];

void
disk_probe_and_init()
{
    memset(&disks, 0, sizeof(disks));

    int res = ata_probe_and_init();
    if (res == -EIO) {
        panic("Failed to probe and initialize disks");
    }
}

struct disk*
disk_get_free_disk()
{
    for (int i = 0; i < LATTE_MAX_DISKS; i++) {
        if (disks[i] == 0) {
            disks[i] = kzalloc(sizeof(struct disk));
            return disks[i];
        }
    }

    return 0;
}