#include "unistd.h"

extern int stdin;
extern int stdout;
extern int stderr;

void
libc_init()
{
    stdout = open("/dev/console", "w");

    return;
}
