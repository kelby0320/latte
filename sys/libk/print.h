#ifndef LIBK_PRINT_H
#define LIBK_PRINT_H

#include <stdarg.h>

int
vsprintk(char *buf, const char *fmt, va_list vlist);

int
sprintk(char *buf, const char *fmt, ...);

int
printk(const char *fmt, ...);

#endif