#ifndef LIBC_STRING_H
#define LIBC_STRING_H

#include <stdbool.h>
#include <stddef.h>

int
memcmp(const void *ptr1, const void *ptr2, size_t n);

void *
memcpy(void *dest, const void *src, size_t n);

void *
memset(void *dest, int ch, size_t n);

char *
strcat(char *dest, const char *src);

int
strcmp(const char *str1, const char *str2);

char *
strcpy(char *dest, const char *src);

size_t
strlen(const char *str);

char *
strncat(char *dest, const char *src, size_t n);

int
strncmp(const char *str1, const char *str2, size_t n);

char *
strncpy(char *dest, const char *src, size_t n);

#endif