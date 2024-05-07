#include "unistd.h"

extern int
main(int argc, char **argv);

int argc;
char **argv;

void
program_entry()
{
    int res = main(argc, argv);

    _exit(res);
}