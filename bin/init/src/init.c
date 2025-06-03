#include "stdio.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "unistd.h"

int
main()
{
    while (1) {
        pid_t pid = fork();
        if ((int)pid < 0) {
            printf("init: Failed to fork\n");
            break;
        } else if (pid == 0) {
            // Child
            execv("/bin/sh", NULL);
        } else {
            // Parent
            int status_code;
            pid_t child_pid = wait(&status_code);
            printf(
                "Child process (%d) exited with status code %d\n", child_pid,
                status_code);
        }
    }

    // Shouldn't reach here
    _exit(EXIT_FAILURE);

    return 0;
}
