#include "drivers/platform/console/console.h"

#include "dev/device.h"
#include "dev/input/input_device.h"
#include "dev/input/keycode.h"
#include "dev/platform/platform_device.h"
#include "drivers/driver.h"
#include "drivers/platform/platform_driver.h"
#include "drivers/platform/vga/vga.h"
#include "errno.h"
#include "fs/devfs/devfs.h"
#include "libk/alloc.h"
#include "libk/print.h"
#include "libk/string.h"

struct file_operations console_fops = {
    .write = console_write,
    .read = console_read
};

struct platform_driver console_drv = {
    .driver = {
	.name = "console",
	.compat = "console"
    },
    .probe = console_probe
};

static void
add_to_input_buffer(struct console_private *private, char c)
{
    if (private->input_buffer_len == CONSOLE_INPUT_BUFFER_SIZE - 1) {
	return; // buffer is full
    }

    private->input_buffer[private->input_buffer_len] = c;
    private->input_buffer_len++;
}

static void
console_input_recv_callback(struct input_device *idev, struct input_event event)
{
    struct console_private *private = console_drv.driver.private;

    if (event.keycode == KEYCODE_LSHIFT || event.keycode == KEYCODE_RSHIFT) {
	if (event.type == INPUT_EVENT_TYPE_KEY_PRESSED) {
	    private->shift_enabled = true;
	} else {
	    private->shift_enabled = false;
	}
    }

    if (event.keycode == KEYCODE_CAPSLOCK) {
	private->capslock_enabled = !private->capslock_enabled;
    }

    bool shift_state = (private->shift_enabled || private->capslock_enabled);
    
    char c = keycode_to_ascii(event.keycode, shift_state);

    if (isalnum(c)) {
	add_to_input_buffer(private, c);
    }
}

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

    struct input_device *kbd_idev = input_device_find("kbd");
    if (!kbd_idev) {
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

    private->kbd_idev = kbd_idev;
    private->input_buffer_len = 0;
    private->shift_enabled = false;
    private->capslock_enabled = false;
    input_device_add_callback(kbd_idev, console_input_recv_callback);

    pdev->device.driver->private = private;

    devfs_make_node(as_device(pdev), &console_fops);

    return 0;
}


