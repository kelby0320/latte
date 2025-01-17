#ifndef DEV_INPUT_H
#define DEV_INPUT_H

#include "device.h"
#include "libk/list.h"

#include <stddef.h>
#include <stdint.h>

#define as_input_device(dev) ((struct input_device *)(dev))

#define INPUT_DEVICE_KEYBOARD_ROWS 8
#define INPUT_DEVICE_KEYBOARD_COLS 24
#define INPUT_DEVICE_KEYBOARD_STATE_SIZE (INPUT_DEVICE_KEYBOARD_ROWS * INPUT_DEVICE_KEYBOARD_COLS)

struct device_driver;
struct input_device;
struct input_event;

typedef uint8_t input_device_type_t;
typedef uint8_t input_event_type_t;
typedef uint8_t input_device_button_state_t;
typedef uint8_t input_device_key_state_t;
typedef void (*input_recv_callback_t)(struct input_device *idev, struct input_event event);

enum {
    INPUT_DEVICE_TYPE_BUTTON,
    INPUT_DEVICE_TYPE_KEYBOARD,
    INPUT_DEVICE_TYPE_MOUSE
};

enum {
    INPUT_EVENT_TYPE_BUTTON_PRESSED,
    INPUT_EVENT_TYPE_BUTTON_RELEASED,
    INPUT_EVENT_TYPE_KEY_PRESSED,
    INPUT_EVENT_TYPE_KEY_RELEASED
};

enum {
    INPUT_DEVICE_BUTTON_STATE_PRESSED,
    INPUT_DEVICE_BUTTON_STATE_RELEASED
};

enum {
    INPUT_DEVICE_KEY_STATE_NULL,
    INPUT_DEVICE_KEY_STATE_PRESSED,
    INPUT_DEVICE_KEY_STATE_RELEASED
};

struct input_event {
    input_event_type_t type;
    uint32_t keycode;
};

struct input_device {
    struct device device;
    const char *name;
    input_device_type_t type;

    struct list_item *input_recv_callbacks;
};

int
input_device_init(struct input_device *idev, const char *name, input_device_type_t type);

int
input_device_add_callback(struct input_device *idev, input_recv_callback_t callback);

int
input_device_register(struct input_device *idev);

struct input_device *
input_device_find(const char *name);
int
input_device_event(struct input_device *idev, struct input_event event);

#endif

