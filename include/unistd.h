#ifndef LIBC_UNISTD_H
#define LIBC_UNISTD_H

#include <stddef.h>

int
open(const char *filename, const char *mode_str);

int
close(int fd);

int
read(int fd, char *ptr, size_t count);

int
write(int fd, const char *ptr, size_t count);

#endif