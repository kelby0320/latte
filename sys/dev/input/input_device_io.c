#include "dev/input/input_device.h"

#include "errno.h"
#include "libk/memory.h"

int
input_device_put_key(struct input_device *idev, char ch)
{
    if (idev->buffer_len == idev->buffer_size) {
	// Move data in buffer to the left by one
	// to make space for new key
	size_t cpy_amt = idev->buffer_size - 1;
	memcpy(idev->buffer, idev->buffer + 1, cpy_amt);

	idev->buffer[idev->buffer_loc] = ch;
	return 0;
    }

    idev->buffer[idev->buffer_loc] = ch;
    idev->buffer_len++;
    idev->buffer_loc++;

    return 0;
}

int
input_device_get_key(struct input_device *idev, char *key)
{
    if (idev->buffer_len == 0) {
	return -ENOENT;
    }
    
    idev->buffer_loc--;
    idev->buffer_len--;

    *key = idev->buffer[idev->buffer_loc];

    idev->buffer[idev->buffer_loc] = 0;

    return 0;
}

int
input_device_flush(struct input_device *idev)
{
    memset(idev->buffer, 0, idev->buffer_size);
    idev->buffer_len = 0;
    idev->buffer_loc = 0;
    return 0;
}
