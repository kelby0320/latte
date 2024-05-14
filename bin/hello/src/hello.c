#include "stdio.h"
#include "unistd.h"

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

    pid_t pid = fork();
    if (pid < 0) {
        printf("Failed to fork\n");
    } else if (pid == 0) {
        // Child
        printf("Child process\n");
    } else {
        // Parent
        printf("Parent process\n");
    }

    return 0;
}