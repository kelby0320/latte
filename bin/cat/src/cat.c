#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#define BUF_SIZE 4096

static void
print_help()
{
    printf("Usage: cat {FILE}\n");
    printf("Concatenate FILE(s) to standard output.\n");
}

int
main(int argc, char **argv)
{
    if (argc < 1) {
        print_help();
        return -1;
    }

    const char *filename = argv[0];

    int fd = open(filename, "r");
    if (fd < 0) {
        printf("cat: %s: No such file or directory\n", filename);
        _exit(EXIT_FAILURE);
    }

    char *buf = malloc(BUF_SIZE);
    if (!buf) {
        printf("cat: Out of memory\n");
        _exit(EXIT_FAILURE);
    }

    int num_read = read(fd, buf, BUF_SIZE - 1);
    if (num_read <= 0) {
        printf("cat: %s: Read error\n", filename);
        _exit(EXIT_FAILURE);
    }

    printf(buf);

    free(buf);

    return 0;
}
