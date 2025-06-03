#ifndef LIBK_STRING_H
#define LIBK_STRING_H

#include <stdbool.h>
#include <stddef.h>

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

char *
strsep(char **stringp, const char *delim);

bool
isalpha(char c);

bool
isdigit(char c);

bool
isalnum(char c);

bool
isupper(char c);

bool
islower(char c);

char
toupper(char c);

char
tolower(char c);

#endif
