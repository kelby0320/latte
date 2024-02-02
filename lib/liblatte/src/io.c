#include "latte/io.h"

extern int
latte_open(const char *filename, const char *mode_str);

extern int
latte_close(int fd);

extern int
latte_read(int fd, char *ptr, size_t count);

extern int
latte_write(int fd, const char *ptr, size_t count);

int
open(const char *filename, const char *mode_str)
{
    return latte_open(filename, mode_str);
}

int
close(int fd)
{
    latte_close(fd);
}

int
read(int fd, char *ptr, size_t count)
{
    return latte_read(fd, ptr, count);
}

int
write(int fd, const char *ptr, size_t count)
{
    return latte_write(fd, ptr, count);
}
