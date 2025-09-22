#ifndef LIBK_PRINT_H
#define LIBK_PRINT_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Write a formatted string to a buffer
 *
 * @param buf       Pointer the the buffer
 * @param fmt       Format string to print
 * @param vlist     Format string argument list
 * @return int      Number of characters written
 */
int
vsprintk(char *buf, const char *fmt, va_list vlist);

/**
 * @brief Write a formatted string to a buffer
 *
 * @param buf       Pointer the the buffer
 * @param fmt       Format string to print
 * @param ...       Format string argument list
 * @return int      Number of characters written
 */
int
sprintk(char *buf, const char *fmt, ...);

/**
 * @brief Write a formatted string to the kernel message buffer
 *
 * @param fmt       Format string
 * @param ...       Format string argument list
 * @return int      Number of characters written
 */
int
printk(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif
