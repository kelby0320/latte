#include "stdio.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "unistd.h"

#include "dirent.h"

#define BUF_SIZE 128

void
read_bin()
{
    printf("Contents of /bin\n");

    DIR *dir = opendir("/bin");
    if (!dir) {
	printf("[ERROR] Could not open /bin");
	return;
    }

    struct dirent *entry = NULL;
    do {
	entry = readdir(dir);
	if (entry) {
	    printf("%s\n", entry->d_name);
	}

	free(entry);
    } while (entry);

    printf("\n");
}

int
main()
{
    char *buf = malloc(BUF_SIZE);

    int fd = open("/latte.txt", "r");
    if (fd < 0) {
        printf("Failed to open file\n");
        _exit(EXIT_FAILURE);
    }

    int num_read = read(fd, buf, BUF_SIZE - 1);
    if (num_read <= 0) {
        printf("Failed to read file\n");
        _exit(EXIT_FAILURE);
    }

    printf(buf);

    read_bin();

    pid_t pid = fork();
    if ((int)pid < 0) {
        printf("Failed to fork\n");
    } else if (pid == 0) {
        // Child
	char *args[] = {"arg1", "arg2", NULL};
        execv("/bin/hello", args);
    } else {
        // Parent
        int status_code;
        pid_t child_pid = wait(&status_code);
        printf("Child process (%d) exited with status code %d\n", child_pid, status_code);
    }

    return 0;
}
