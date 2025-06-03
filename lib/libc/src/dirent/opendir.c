#include "dirent.h"

#include "stdlib.h"
#include "syscall.h"

DIR *
opendir(const char *dirname)
{
    struct _dir *_dir = malloc(sizeof(struct _dir));
    if (!_dir) {
        return NULL;
    }

    int dir_fd = do_syscall1(OPENDIR_SYSCALL_NO, (int)dirname);
    if (dir_fd < 0) {
        goto err_opendir;
    }

    _dir->fd = dir_fd;
    return _dir;

err_opendir:
    free(_dir);
    return NULL;
}
