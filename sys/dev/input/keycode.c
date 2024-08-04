#include "dev/input/keycode.h"

#include "libk/string.h"

static uint8_t keycode_to_ascii_tbl[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '0', '-', '=', 0x8, 0, 0,
    0, 0, '/', '*', '-', '\t', 'q', 'w',
    'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']', '\\', 0x7f, 0, 0, '7', '8',
    '9', '+', 0, 'a', 's', 'd', 'f', 'g',
    'h', 'j', 'k', 'l', ';', '\'', '\n', '4',
    '5', '6', 0, 'z', 'x', 'c', 'v', 'b',
    'n', 'm', ',', '.', '/', 0, 0, '1',
    '2', '3', '\n', 0, 0, 0, ' ', 0,
    0, 0, 0, 0, 0, 0, '0', 0x7f
};

char
keycode_to_ascii(unsigned int keycode, bool shift_enabled)
{
    if (keycode >= sizeof(keycode_to_ascii_tbl)) {
	return 0;
    }
    
    char c = keycode_to_ascii_tbl[keycode];

    if (islower(c) && shift_enabled) {
	c =  toupper(c);
    }

    return c;
}
