#ifndef LIBC_CTYPE_H
#define LIBC_CTYPE_H

#include <stdbool.h>

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

#endif