#include "latte/io.h"

extern int stdin;
extern int stdout;
extern int stderr;

void
libc_init()
{
    stdout = open("/dev/term0", "w");

    return;
}