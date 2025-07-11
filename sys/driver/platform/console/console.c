#include "driver/platform/console.h"

#include "dev/input_device.h"
#include "dev/keycode.h"
#include "dev/platform_device.h"
#include "device.h"
#include "driver.h"
#include "driver/platform/vga.h"
#include "driver/platform_driver.h"
#include "errno.h"
#include "fs/devfs.h"
#include "libk/alloc.h"
#include "libk/print.h"
#include "libk/string.h"

struct file_operations console_fops = {
    .write = console_write, .read = console_read};

struct platform_driver console_drv = {
    .driver = {.name = "console", .compat = "console"}, .probe = console_probe};

static bool
is_valid_console_char(char c)
{
    return c >= 32 && c <= 126;
}

static int
add_to_input_buffer(struct console_private *private, char c)
{
    if (private->input_buffer_len == CONSOLE_INPUT_BUFFER_SIZE - 1) {
        return 0; // buffer is full
    }

    private->input_buffer[private->input_buffer_len] = c;
    private->input_buffer_len++;

    return 1;
}

static int
remove_from_input_buffer(struct console_private *private)
{
    if (private->input_buffer_len == 0) {
        return 0;
    }

    private->input_buffer[private->input_buffer_len - 1] = 0;
    private->input_buffer_len--;

    return 1;
}

static void
console_input_recv_callback(struct input_device *idev, struct input_event event)
{
    struct console_private *private = console_drv.driver.private;

    // Handle shift press/release
    if (event.keycode == KEYCODE_LSHIFT || event.keycode == KEYCODE_RSHIFT) {
        if (event.type == INPUT_EVENT_TYPE_KEY_PRESSED) {
            private->shift_enabled = true;
        } else {
            private->shift_enabled = false;
        }

        return;
    }

    // Handle capslock toggle
    if (event.keycode == KEYCODE_CAPSLOCK) {
        private->capslock_enabled = !private->capslock_enabled;

        return;
    }

    // Handle key pressed event
    if (event.type == INPUT_EVENT_TYPE_KEY_PRESSED) {
        /* private->input_ready = false; */

        bool shift_state =
            (private->shift_enabled || private->capslock_enabled);
        char c = keycode_to_ascii(event.keycode, shift_state);

        if (c == 8) { // Delete key
            int key_removed = remove_from_input_buffer(private);
            if (key_removed == 1) {
                console_write(as_device(private->console_device), 0, &c, 1);
            }

            return;
        }

        // Add character to input buffer
        if (is_valid_console_char(c)) {
            add_to_input_buffer(private, c);
        }

        if (c == '\n') {
            private->input_ready = true;
        }

        console_write(as_device(private->console_device), 0, &c, 1);
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
        printk("console_probe - failed to find vga device\n");
        return -EAGAIN;
    }

    struct input_device *kbd_idev = input_device_find("kbd");
    if (!kbd_idev) {
        printk("console_probe - failed to find kbd device\n");
        return -EAGAIN;
    }

    struct console_private *private = kzalloc(sizeof(struct console_private));
    if (!private) {
        printk("console_probe - failed to allocate console_private\n");
        return -ENOMEM;
    }

    private->console_device = pdev;

    private->vga = vga;
    private->cur_col = 0;
    private->cur_row = 0;
    private->max_col = vga_max_column(vga);
    private->max_row = vga_max_row(vga);
    private->color = vga_current_color(vga);

    private->kbd_idev = kbd_idev;
    private->input_buffer_len = 0;
    private->input_ready = false;
    private->shift_enabled = false;
    private->capslock_enabled = false;
    input_device_add_callback(kbd_idev, console_input_recv_callback);

    pdev->device.driver->private = private;

    int res = devfs_make_node(as_device(pdev), &console_fops);
    if (res < 0) {
        printk("Failed to add devfs node for console - %d\n", res);
    }

    return 0;
}
