#ifndef INPUT_DEVICE_KEYCODE_H
#define INPUT_DEVICE_KEYCODE_H

#include <stdbool.h>
#include <stdint.h>

enum {
    KEYCODE_ESC,
    KEYCODE_F1,
    KEYCODE_F2,
    KEYCODE_F3,
    KEYCODE_F4,
    KEYCODE_F5,
    KEYCODE_F6,
    KEYCODE_F7,
    KEYCODE_F8,
    KEYCODE_F9,
    KEYCODE_F10,
    KEYCODE_F11,
    KEYCODE_F12,
    KEYCODE_PRINTSCREEN,
    KEYCODE_SCROLLLOCK,
    KEYCODE_PAUSE,
    KEYCODE_BACKTICK,
    KEYCODE_1,
    KEYCODE_2,
    KEYCODE_3,
    KEYCODE_4,
    KEYCODE_5,
    KEYCODE_6,
    KEYCODE_7,
    KEYCODE_8,
    KEYCODE_9,
    KEYCODE_0,
    KEYCODE_DASH,
    KEYCODE_EQU,
    KEYCODE_BACKSPACE,
    KEYCODE_INSERT,
    KEYCODE_HOME,
    KEYCODE_PGUP,
    KEYCODE_NUMLOCK,
    KEYCODE_NP_FSLASH,
    KEYCODE_NP_STAR,
    KEYCODE_NP_DASH,
    KEYCODE_TAB,
    KEYCODE_Q,
    KEYCODE_W,
    KEYCODE_E,
    KEYCODE_R,
    KEYCODE_T,
    KEYCODE_Y,
    KEYCODE_U,
    KEYCODE_I,
    KEYCODE_O,
    KEYCODE_P,
    KEYCODE_LBRACKET,
    KEYCODE_RBRACKET,
    KEYCODE_BACKSLASH,
    KEYCODE_DEL,
    KEYCODE_END,
    KEYCODE_PGDOWN,
    KEYCODE_NP_7,
    KEYCODE_NP_8,
    KEYCODE_NP_9,
    KEYCODE_NP_PLUS,
    KEYCODE_CAPSLOCK,
    KEYCODE_A,
    KEYCODE_S,
    KEYCODE_D,
    KEYCODE_F,
    KEYCODE_G,
    KEYCODE_H,
    KEYCODE_J,
    KEYCODE_K,
    KEYCODE_L,
    KEYCODE_SEMICOLON,
    KEYCODE_SINGLEQUOTE,
    KEYCODE_ENTER,
    KEYCODE_NP_4,
    KEYCODE_NP_5,
    KEYCODE_NP_6,
    KEYCODE_LSHIFT,
    KEYCODE_Z,
    KEYCODE_X,
    KEYCODE_C,
    KEYCODE_V,
    KEYCODE_B,
    KEYCODE_N,
    KEYCODE_M,
    KEYCODE_COMMA,
    KEYCODE_PERIOD,
    KEYCODE_FORWARDSLASH,
    KEYCODE_RSHIFT,
    KEYCODE_ARROWUP,
    KEYCODE_NP_1,
    KEYCODE_NP_2,
    KEYCODE_NP_3,
    KEYCODE_NP_ENTER,
    KEYCODE_LCTRL,
    KEYCODE_LSUPER,
    KEYCODE_LALT,
    KEYCODE_SPACE,
    KEYCODE_RALT,
    KEYCODE_RSUPER,
    KEYCODE_CONTEXT,
    KEYCODE_RCTRL,
    KEYCODE_ARROWLEFT,
    KEYCODE_ARROWDOWN,
    KEYCODE_ARROWRIGHT,
    KEYCODE_NP_0,
    KEYCODE_NP_PERIOD
};

char
keycode_to_ascii(unsigned int keycode, bool shift_enabled);

#endif
