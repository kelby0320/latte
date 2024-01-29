#include "dev/vga/vga.h"

#include "libk/string.h"

int
vga_device_init(struct vga_device *vga_device)
{
    strcpy(vga_device->device.name, "vga0");

    return 0;
}

int
vga_device_set_mode_3(struct vga_device *vga_device)
{
    // TODO
    return 0;
}

int
vga_device_set_mode_13(struct vga_device *vga_device)
{
    // TODO
    return 0;
}

void *
vga_device_get_framebuffer(struct vga_device *vga_device)
{
    // For now return the normal VGA framebuffer address
    return (void *)0xB8000;
}