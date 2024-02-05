#include "latte/io.h"

extern int stdin;
extern int stdout;
extern int stderr;

void
libc_init(int argc, char **argv, int envc, char **envp)
{
    stdout = open("dev/term0", "w");

    return;
}