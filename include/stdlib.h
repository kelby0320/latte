#ifndef LIBC_STDLIB_H
#define LIBC_STDLIB_H

#include <stddef.h>

void *
malloc(size_t size);

void
free(void *ptr);

char *
getenv(const char *name);

int
putenv(char *string);

int
setenv(const char *name, const char *value, int overwrite);

#endif