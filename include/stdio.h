#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include <stdarg.h>

extern int stdin;
extern int stdout;
extern int stderr;

int
vsprintf(char *buf, const char *fmt, va_list vlist);

int
sprintf(char *buf, const char *fmt, ...);

int
printf(const char *fmt, ...);

#endif