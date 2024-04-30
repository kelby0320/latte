#ifndef LIBC_UNISTD_H
#define LIBC_UNISTD_H

#include <stddef.h>
#include <stdint.h>

typedef uint32_t pid_t;

int
open(const char *filename, const char *mode_str);

int
close(int fd);

int
read(int fd, char *ptr, size_t count);

int
write(int fd, const char *ptr, size_t count);

pid_t
fork();

int
execv(const char *path, char *const *argv);

void
_exit(int status);

#endif