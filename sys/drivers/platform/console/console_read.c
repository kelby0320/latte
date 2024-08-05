#include "drivers/platform/console/console.h"

#include "dev/device.h"
#include "drivers/driver.h"
#include "libk/memory.h"

int
console_read(struct device *dev, size_t offset, char *buf, size_t count)
{
    struct console_private *private = dev->driver->private;

    if (count > private->input_buffer_len) {
	count = private->input_buffer_len;
    }

    memcpy(buf, private->input_buffer, count);

    return count;
}
