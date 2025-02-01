#include "dirent.h"

#include "stdlib.h"
#include "syscall.h"

struct dirent *
readdir(DIR *dirp)
{
    struct dirent *entry = malloc(sizeof(struct dirent));
    if (!entry) {
        return NULL;
    }

    int res = do_syscall2(READDIR_SYSCALL_NO, (int)dirp->fd, (int)entry);
    if (res < 0) {
        goto err_readdir;
    }

    return entry;

err_readdir:
    free(entry);
    return NULL;
}
