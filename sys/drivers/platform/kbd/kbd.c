#include "drivers/platform/kbd/kbd.h"

#include "cpu/port.h"
#include "dev/input/input_device.h"
#include "dev/platform/platform_device.h"
#include "drivers/platform/kbd/ps2.h"
#include "drivers/platform/kbd/scancode.h"
#include "drivers/platform/platform_driver.h"
#include "errno.h"
#include "irq/irq.h"
#include "libk/alloc.h"
#include "libk/print.h"

#define KBD_INTERRUPT_NO             0x21
#define KBD_SC_KEY_RELEASED 0xF0

static struct kbd_private kbd_private = {
    .key_release = false,
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
    uint8_t scancode = ps2_read_data();
    ps2_read_data();

    if (scancode == KBD_SC_KEY_RELEASED) {
	kbd_private.key_release = true;
	return;
    }

    unsigned int keycode = scancode_to_keycode(scancode);

    struct input_event event = {
	    .type = INPUT_EVENT_TYPE_KEY_PRESSED,
	    .keycode = keycode	    
	};

    if (kbd_private.key_release) {
	event.type = INPUT_EVENT_TYPE_KEY_RELEASED;
    }

    kbd_private.key_release = false;

    input_device_event(kbd_private.idev, event);
}

static int
kbd_ps2_init()
{
    /* Disable PS2 ports */
    ps2_write_cmd(PS2_CMD_DIS_PORT1);
    ps2_write_cmd(PS2_CMD_DIS_PORT2);

    /* Flush output buffer */
    ps2_read_data();

    /* Write initial configuration */
    uint8_t config = 0x0;
    ps2_write_cmd(PS2_CMD_WRITE_CFG);
    ps2_write_data(config);

    /* Controller self test */
    ps2_write_cmd(PS2_CMD_CTRL_SELF_TEST);
    uint8_t test_result = ps2_read_data();
    if (test_result != PS2_CTRL_SELF_TEST_PASS) {
	printk("PS2 controller failed self test with result %d\n", test_result);
	return -EIO;
    }

    /* Test port 1 */
    ps2_write_cmd(PS2_CMD_TEST_PORT1);
    test_result = ps2_read_data();
    if (test_result != 0x0) {
	printk("PS2 port 1 failed self test with result %d\n", test_result);
	return -EIO;
    }

    /* Test port 2 */
    ps2_write_cmd(PS2_CMD_TEST_PORT2);
    test_result = ps2_read_data();
    if (test_result != 0x0) {
	printk("PS2 port 2 failed self test with result %d\n", test_result);
    }

    /* Enable port 1 */
    ps2_write_cmd(PS2_CMD_EN_PORT1);
    ps2_write_cmd(PS2_CMD_READ_CFG);
    config = ps2_read_data();
    config |= PS2_CFG_PORT1_INT_EN;
    ps2_write_cmd(PS2_CMD_WRITE_CFG);
    ps2_write_data(config);

    /* Reset device 1 */
    ps2_write_data(0xFF);
    uint8_t status = ps2_read_data();
    uint8_t type = ps2_read_data();

    return 0;
}

int
kbd_drv_init()
{
    platform_driver_register(&kbd_drv);

    return 0;
}

int
kbd_probe(struct platform_device *dev)
{
    struct input_device *idev = kzalloc(sizeof(struct input_device));
    if (!idev) {
	printk("kbd_probe - FAILED to allocate input_device\n");
	return -ENOMEM;
    }

    int res = input_device_init(idev, "kbd", INPUT_DEVICE_TYPE_KEYBOARD);
    if (res < 0) {
	printk("kbd_probe - input_device_init FAILED\n");
	goto err_input;
    }

    kbd_private.idev = idev;

    res = kbd_ps2_init();
    if (res < 0) {
	printk("kbd_probe - ps2_init FAILED\n");
	goto err_ps2;
    }

    res = register_irq(KBD_INTERRUPT_NO, kbd_interrupt_handler);
    if (res < 0) {
	printk("kbd_probe - register_irq FAILED\n");
    }

    res = input_device_register(idev);
    if (res < 0) {
	printk("kbd_probe - input_device_register FAILED\n");
    }

    return 0;

err_ps2:
    kbd_private.idev = NULL;

err_input:
    kfree(idev);
    
    return res;
}
