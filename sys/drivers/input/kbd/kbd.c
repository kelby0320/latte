#include "drivers/input/kbd/kbd.h"

#include "cpu/port.h"
#include "dev/input/input_device.h"
#include "drivers/input/input_driver.h"
#include "drivers/input/kbd/scancode.h"
#include "errno.h"
#include "irq/irq.h"

#define KBD_INTERRUPT_NO             0x21
#define KBD_PS2_DATA_PORT            0x60
#define KBD_PS2_CMD_PORT             0x64
#define KBD_PS2_CMD_EN_FIRST_PORT    0xAE

#define KBD_SC_KEY_RELEASED 0x80
#define KBD_SC_CAPSLOCK     0x3A

static struct kbd_private kbd_private = {
    .caps_lock = false,
    .scancode_set = scancode_set_one
};

struct input_driver    kbd_drv = {
    .driver = {
	.name = "kbd",
	.compat = "kbd"
    },
    .private = &kbd_private,
    .probe = kbd_probe
};

static char
scancode_to_char(uint8_t scancode)
{
    size_t scanset_size = sizeof(scancode_set_two) / sizeof(uint8_t);
    if (scancode > scanset_size) {
	return 0;
    }

    char c = scancode_set_two[scancode];
    if (!kbd_private.caps_lock) {
	if (c >= 'A' && c <= 'Z') {
	    c += 32;
	}
    }

    return c;
}

static void
kbd_interrupt_handler()
{
    uint8_t scancode = insb(KBD_PS2_DATA_PORT);
    insb(KBD_PS2_DATA_PORT);

    if (scancode & KBD_SC_KEY_RELEASED) {
	return;
    }

    if (scancode == KBD_SC_CAPSLOCK) {
	kbd_private.caps_lock = !kbd_private.caps_lock;
	return;
    }

    char c = scancode_to_char(scancode);

    if (c != 0) {
	input_device_put_key(kbd_private.idev, c);
    }
}


int
kbd_drv_init()
{
    input_driver_register(&kbd_drv);

    return 0;
}

int
kbd_probe(struct device *dev)
{
    struct input_device *idev = as_input_device(dev);
    kbd_private.idev = idev;

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

    register_irq(KBD_INTERRUPT_NO, kbd_interrupt_handler);

    return 0;
}
