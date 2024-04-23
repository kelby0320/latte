#include "latte/io.h"
#include "latte/mmap.h"
#include "stdio.h"

#define BUF_SIZE 128

int
main(int argc, char **argv)
{
    char *buf = mmap(BUF_SIZE);

    int fd = open("/latte.txt", "r");
    if (fd < 0) {
        printf("Failed to open file\n");
        while (1) {}
    }

    int num_read = read(fd, buf, BUF_SIZE - 1);
    if (num_read <= 0) {
        printf("Failed to read file\n");
        while (1) {}
    }

    printf(buf);

    while (1) {}

    return 0;
}