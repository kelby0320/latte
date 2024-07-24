#ifndef LIBC_DIRENT_H
#define LIBC_DIRENT_H

#include <stdint.h>

struct _dir {
    unsigned int fd;
};

typedef struct _dir DIR;

struct dirent {
    uint32_t d_ino;
    char d_name[256];
};

DIR *
opendir(const char *dirname);

int
closedir(DIR *dirp);

struct dirent *
readdir(DIR *dirp);

#endif
