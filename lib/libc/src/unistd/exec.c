#include "unistd.h"

#include "syscall.h"

int
execv(const char *path, char *const *argv)
{
    int argv_len = 0;
    if (argv) {
        for (argv_len = 0; argv[argv_len]; argv_len++)
            ;
    }

    return do_syscall3(EXECV_SYSCALL_NO, (int)path, (int)argv, argv_len);
}
