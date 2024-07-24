#include "dirent.h"

#include "stdlib.h"
#include "syscall.h"

int
closedir(DIR *dirp)
{
    int res = do_syscall1(CLOSEDIR_SYSCALL_NO, (int)dirp->fd);
    free(dirp);
    return res;
}
