#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include <stdarg.h>

extern int stdin;
extern int stdout;
extern int stderr;

/**
 * @brief Write a formatted string to a buffer
 *
 * @param buf       Pointer the the buffer
 * @param fmt       Format string to print
 * @param vlist     Format string argument list
 * @return int      Number of characters written
 */
int
vsprintf(char *buf, const char *fmt, va_list vlist);

/**
 * @brief Write a formatted string to a buffer
 *
 * @param buf       Pointer the the buffer
 * @param fmt       Format string to print
 * @param ...       Format string argument list
 * @return int      Number of characters written
 */
int
sprintf(char *buf, const char *fmt, ...);

/**
 * @brief Write a formatted string to stdout
 *
 * @param fmt       Format string
 * @param ...       Format string argument list
 * @return int      Number of characters written
 */
int
printf(const char *fmt, ...);

#endif