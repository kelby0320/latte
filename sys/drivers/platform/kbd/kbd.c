#include "drivers/platform/kbd/kbd.h"

#include "cpu/port.h"
#include "dev/input/input_device.h"
#include "dev/platform/platform_device.h"
#include "drivers/platform/kbd/scancode.h"
#include "drivers/platform/platform_driver.h"
#include "errno.h"
#include "irq/irq.h"
#include "libk/alloc.h"

#define KBD_INTERRUPT_NO             0x21
#define KBD_PS2_DATA_PORT            0x60
#define KBD_PS2_CMD_PORT             0x64
#define KBD_PS2_CMD_EN_FIRST_PORT    0xAE

#define KBD_SC_KEY_RELEASED 0x80
#define KBD_SC_CAPSLOCK     0x3A

static struct kbd_private kbd_private = {
    .caps_lock = false,
    .scancode_set = scancode_set_two
};

struct platform_driver kbd_drv = {
    .driver = {
	.name = "kbd",
	.compat = "kbd"
    },
    .private = &kbd_private,
    .probe = kbd_probe
};

static unsigned int
scancode_to_keycode(uint8_t scancode)
{
    size_t scanset_size = sizeof(scancode_set_two) / sizeof(uint8_t);
    if (scancode > scanset_size) {
	return 0;
    }

    return scancode_set_two[scancode];
}

static void
kbd_interrupt_handler()
{
    uint8_t scancode = insb(KBD_PS2_DATA_PORT);
    insb(KBD_PS2_DATA_PORT);

    unsigned int keycode= scancode_to_keycode(scancode);

    struct input_event event = {
	    .type = INPUT_EVENT_TYPE_KEY_PRESSED,
	    .keycode = keycode	    
	};

    if (scancode & KBD_SC_KEY_RELEASED) {
	event.type = INPUT_EVENT_TYPE_KEY_RELEASED;
    }

    input_device_event(kbd_private.idev, event);
}

static int
ps2_init()
{
    /* Disable PS2 devices */
    outb(KBD_PS2_CMD_PORT, 0xAD);
    outb(KBD_PS2_CMD_PORT, 0xA7);

    /* Flush output buffer */
    insb(KBD_PS2_DATA_PORT);

    /* Read configuration byte */
    outb(KBD_PS2_CMD_PORT, 0x20);
    uint8_t config = insb(KBD_PS2_DATA_PORT);

    /* Write configuration byte */
    config &= 0b00100110;
    outb(KBD_PS2_CMD_PORT, 0x60);
    outb(KBD_PS2_DATA_PORT, config);

    /* Controller self test */
    outb(KBD_PS2_CMD_PORT, 0xAA);
    uint8_t test_result = insb(KBD_PS2_DATA_PORT);
    if (test_result != 0x55) {
	return -EIO;
    }

    /* Check for second channel */
    outb(KBD_PS2_CMD_PORT, 0xA8);
    outb(KBD_PS2_CMD_PORT, 0x20);
    config = insb(KBD_PS2_DATA_PORT);
    if (config & 0x10) {
	return -EIO;
    }

    /* Disable the second port */
    outb(KBD_PS2_CMD_PORT, 0xA7);
    outb(KBD_PS2_CMD_PORT, 0x20);
    config = insb(KBD_PS2_DATA_PORT);
    config &= 0b01010101;
    outb(KBD_PS2_CMD_PORT, 0x60);
    outb(KBD_PS2_DATA_PORT, config);

    /* Test port 1 */
    outb(KBD_PS2_CMD_PORT, 0xAB);
    test_result = insb(KBD_PS2_DATA_PORT);
    if (test_result != 0x0) {
	return -EIO;
    }

    /* Test port 2 */
    outb(KBD_PS2_CMD_PORT, 0xA9);
    test_result = insb(KBD_PS2_DATA_PORT);
    if (test_result != 0x0) {
	return -EIO;
    }

    /* Enable ports 1 and 2 */
    outb(KBD_PS2_CMD_PORT, 0xAE);
    outb(KBD_PS2_CMD_PORT, 0xA8);
    outb(KBD_PS2_CMD_PORT, 0x20);
    config = insb(KBD_PS2_DATA_PORT);
    config |= 0b00110011;
    outb(KBD_PS2_CMD_PORT, 0x60);
    outb(KBD_PS2_DATA_PORT, config);

    /* Reset device 1 */
    outb(KBD_PS2_DATA_PORT, 0xFF);
    uint8_t status = insb(KBD_PS2_DATA_PORT);
    uint8_t type = insb(KBD_PS2_DATA_PORT);

    /* Reset device 2 */
    outb(KBD_PS2_CMD_PORT, 0xD4);
    outb(KBD_PS2_DATA_PORT, 0xFF);
    status = insb(KBD_PS2_DATA_PORT);
    type = insb(KBD_PS2_DATA_PORT);

    return 0;
}


int
kbd_drv_init()
{
    platform_driver_register(&kbd_drv);

    return 0;
}

int
kbd_probe(struct device *dev)
{
    struct input_device *idev = kzalloc(sizeof(struct input_device));
    if (!idev) {
	return -ENOMEM;
    }

    int res = input_device_init(idev, "kbd", INPUT_DEVICE_TYPE_KEYBOARD);
    if (res < 0) {
	goto err_input;
    }

    kbd_private.idev = idev;

    res = ps2_init();
    if (res < 0) {
	goto err_ps2;
    }

    register_irq(KBD_INTERRUPT_NO, kbd_interrupt_handler);

    return 0;

err_ps2:
    kbd_private.idev = NULL;

err_input:
    kfree(idev);
    
    return res;
}
