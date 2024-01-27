#include "libk/print.h"

#include "errno.h"
#include "libk/string.h"
#include "msgbuf.h"

static int
int_to_str(int i, char *buf, size_t buf_size)
{
    int loc = buf_size - 1;
    size_t buf_len = 0;
    buf[loc] = 0; // Null terminator
    bool neg = true;

    if (i >= 0) {
        neg = false;
        i = -i;
    }

    // Build the string backwards from least to most significate digit
    while (i) {
        buf[--loc] = '0' - (i % 10);
        i /= 10;
        buf_len++;
    }

    // Special case when i == 0
    if (loc == (int)buf_size - 1) {
        buf[--loc] = '0';
        buf_len++;
    }

    // Append minus sign if negative
    if (neg) {
        buf[--loc] = '-';
        buf_len++;
    }

    // Copy the integer string to the begining of buf
    strcpy(buf, &buf[loc]);

    return buf_len;
}

static int
append_int_to_buf(char **buf, int ival)
{
    char int_buf[16];
    int int_buf_len = int_to_str(ival, int_buf, 16);
    strcat(*buf, int_buf);
    *buf += int_buf_len;
    return int_buf_len;
}

static int
append_str_to_buf(char **buf, char *sval)
{
    int sval_len = strlen(sval);
    strcat(*buf, sval);
    *buf += sval_len;
    return sval_len;
}

int
vsprintk(char *buf, const char *fmt, va_list vlist)
{
    const char *fmt_ptr;
    char *buf_ptr;
    int buf_len = 0;
    int ival;
    char *sval;

    for (fmt_ptr = fmt, buf_ptr = buf; *fmt_ptr; fmt_ptr++) {
        if (*fmt_ptr != '%') {
            *buf_ptr = *fmt_ptr;
            buf_ptr++;
            buf_len++;
            continue;
        }

        // *fmt_ptr == '%'
        // Look at the following character to determine what to do
        fmt_ptr++;

        switch (*fmt_ptr) {
        case 'd':
            ival = va_arg(vlist, int);
            buf_len += append_int_to_buf(&buf_ptr, ival);
            break;
        case 's':
            sval = va_arg(vlist, char *);
            buf_len += append_str_to_buf(&buf_ptr, sval);
            break;
        default:
            *buf_ptr = *fmt_ptr;
            buf_ptr++;
        }
    }

    *buf_ptr = '\0';

    return buf_len;
}

int
sprintk(char *buf, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    return vsprintk(buf, fmt, args);
}

int
printk(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[128];

    vsprintk(buf, fmt, args);

    msgbuf_write(buf);
}
