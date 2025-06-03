#ifndef KBD_PS2_H
#define KBD_PS2_H

#include <stdint.h>

#define PS2_INTERRUPT_NO 0x21
#define PS2_DATA_PORT    0x60
#define PS2_STATUS_PORT  0x64
#define PS2_CMD_PORT     0x64

#define PS2_CMD_READ_CFG       0x20
#define PS2_CMD_WRITE_CFG      0x60
#define PS2_CMD_DIS_PORT1      0xAD
#define PS2_CMD_EN_PORT1       0xAE
#define PS2_CMD_DIS_PORT2      0xA7
#define PS2_CMD_EN_PORT2       0xA8
#define PS2_CMD_CTRL_SELF_TEST 0xAA
#define PS2_CMD_TEST_PORT1     0xAB
#define PS2_CMD_TEST_PORT2     0xA9

#define PS2_STATUS_OUT_RDY     0b00000001
#define PS2_STATUS_IN_RDY      0b00000010
#define PS2_STATUS_SYS_FLAG    0b00000100
#define PS2_STATUS_CMD_DATA    0b00001000
#define PS2_STATUS_TIMEOUT_ERR 0b01000000
#define PS2_STATUS_PARITY_ERR  0b10000000

#define PS2_CFG_PORT1_INT_EN   0b00000001
#define PS2_CFG_PORT2_INT_EN   0b00000010
#define PS2_CFG_SYSTEM_POST    0b00000100
#define PS2_CFG_PORT1_CLK_DIS  0b00010000
#define PS2_CFG_PORT2_CLK_DIS  0b00100000
#define PS2_CFG_PORT1_TRANS_EN 0b01000000

#define PS2_CTRL_SELF_TEST_PASS 0x55

enum ps2_port { PS2_PORT1 = 1, PS2_PORT2 = 2 };

typedef enum ps2_port ps2_port_t;

void
ps2_write_cmd(uint8_t cmd);

void
ps2_write_data(uint8_t data);

uint8_t
ps2_read_data();

#endif
