#include "drivers/platform/console/console.h"

#include "dev/device.h"
#include "drivers/driver.h"
#include "errno.h"
#include "libk/memory.h"

int
console_read(struct device *dev, size_t offset, char *buf, size_t count)
{
    struct console_private *private = dev->driver->private;

    if (!private->input_ready) {
	return -EAGAIN;
    }

    if (count > private->input_buffer_len) {
	count = private->input_buffer_len;
    }

    // Copy input data to buf
    memcpy(buf, private->input_buffer, count);

    // Re-adjust input buffer to start at 0
    int shift_amt = CONSOLE_INPUT_BUFFER_SIZE - count;
    memcpy(private->input_buffer, private->input_buffer + count, shift_amt);

    // Reset remaining elements to 0
    memset(private->input_buffer + shift_amt, 0, count);

    private->input_buffer_len -= count;
    private->input_ready = false;
    
    return count;
}
