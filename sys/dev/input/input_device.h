#ifndef DEV_INPUT_H
#define DEV_INPUT_H

#include "dev/device.h"

#include <stddef.h>
#include <stdint.h>

#define as_input_device(dev) ((struct input_device *)(dev))

struct device_driver;

typedef unsigned int input_device_type_t;

enum {
    INPUT_DEVICE_TYPE_BUTTON,
    INPUT_DEVICE_TYPE_KEYBOARD,
    INPUT_DEVICE_TYPE_MOUSE
};

struct input_device {
    struct device device;
    const char *name;
    input_device_type_t type;

    /* The input buffer */
    char *buffer;

    /* The size of the input buffer */
    size_t buffer_size;

    /* The current length (# of keys in) of the input buffer */
    size_t buffer_len;

    /* The location to write the next key in the input buffer */
    unsigned int buffer_loc;
};

int
input_bus_init();

struct bus *
input_bus_get_bus();

int
input_add_devices();

int
input_match(struct device *dev, struct device_driver *drv);

int
input_probe(struct device *dev);

int
input_device_register(struct input_device *idev);

struct input_device *
input_device_find(const char *name);

int
input_device_put_key(struct input_device *idev, char ch);

int
input_device_get_key(struct input_device *idev, char *key);

int
input_device_flush(struct input_device *idev);

#endif

