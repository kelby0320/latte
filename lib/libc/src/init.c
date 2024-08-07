#include "unistd.h"

extern int stdin;
extern int stdout;
extern int stderr;

void
libc_init()
{
    stdin = open("/dev/console", "r");
    stdout = open("/dev/console", "w");
    stderr = stdout;

    return;
}
