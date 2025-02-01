#include "stdio.h"
#include "unistd.h"

static int
readline(char *buf, size_t len)
{
    int num_read = read(stdin, buf, len);
    if (buf[num_read - 1] == '\n') {
        buf[num_read - 1] = 0;
        num_read--;
    }

    return num_read;
}

static void
hello_args(int argc, char **argv)
{
    printf("Hello ");

    for (int i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }

    printf("\n");
}

int
main(int argc, char **argv)
{
    if (argc > 0) {
        hello_args(argc, argv);
        return 0;
    }

    printf("What is your name? ");

    char buf[128] = {0};
    readline(buf, 128);

    printf("\nHello %s\n", buf);

    return 0;
}
