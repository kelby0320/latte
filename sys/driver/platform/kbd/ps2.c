#include "driver/platform/kbd/ps2.h"

#include "errno.h"
#include "libk/print.h"
#include "port.h"

#include <stdint.h>

#define PS2_POLL_LIMIT 1000

/**
 * @brief Wait for the PS2 input buffer to be empty
 */
static void
ps2_wait_for_input(uint8_t port)
{
    int i = 0;
    do {
        uint8_t status = insb(PS2_STATUS_PORT);
        if ((status & PS2_STATUS_IN_RDY) == 0) {
            break;
        }

        uint8_t data = insb(port);
        i++;
    } while (i < PS2_POLL_LIMIT);
}

/**
 * @brief Wait for the PS2 output buffer to by full
 */
static void
ps2_wait_for_output()
{
    int i = 0;
    do {
        uint8_t status = insb(PS2_STATUS_PORT);
        if (status & PS2_STATUS_OUT_RDY) {
            break;
        }
        i++;
    } while (i < PS2_POLL_LIMIT);
}

void
ps2_write_cmd(uint8_t cmd)
{
    ps2_wait_for_input(PS2_CMD_PORT);
    outb(PS2_CMD_PORT, cmd);
}

void
ps2_write_data(uint8_t data)
{
    ps2_wait_for_input(PS2_DATA_PORT);
    outb(PS2_DATA_PORT, data);
}

uint8_t
ps2_read_data()
{
    ps2_wait_for_output();
    return insb(PS2_DATA_PORT);
}
