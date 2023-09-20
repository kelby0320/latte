#ifndef LIBK_STRING_H
#define LIBK_STRING_H

#include <stddef.h>

char*
strcat(char *dest, const char *src);

int
strcmp(const char *str1, const char *str2);

char*
strcpy(char *dest, const char *src);

size_t
strlen(const char *str);

char*
strncat(char *dest, const char *src, size_t n);

int
strncmp(const char *str1, const char *str2, size_t n);

char*
strncpy(char *dest, const char *src, size_t n);

#endif
