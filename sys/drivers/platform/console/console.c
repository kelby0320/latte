#include "drivers/platform/console/console.h"

#include "dev/device.h"
#include "dev/platform/platform_device.h"
#include "drivers/driver.h"
#include "drivers/platform/platform_driver.h"
#include "errno.h"
#include "fs/devfs/devfs.h"
#include "libk/alloc.h"

struct file_operations console_fops = {
    .write = console_write
};

struct platform_driver console_drv = {
    .driver = {
	.name = "console",
	.compat = "console"
    },
    .probe = console_probe
};

int
console_drv_init()
{
    platform_driver_register(&console_drv);

    return 0;
}

int
console_probe(struct platform_device *pdev)
{
    struct device *vga = as_device(platform_device_find("vga"));
    if (!vga) {
	return -EAGAIN;
    }

    struct console_private *private = kzalloc(sizeof(struct console_private));
    if (!private) {
	return -ENOMEM;
    }

    private->vga = vga;
    private->cur_col = 0;
    private->cur_row = 0;
    private->max_col = vga_max_column(vga);
    private->max_row = vga_max_row(vga);
    private->color = vga_current_color(vga);

    pdev->device.driver->private = private;

    devfs_make_node(as_device(pdev), &console_fops);

    return 0;
}


