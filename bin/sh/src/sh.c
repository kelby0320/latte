#include "stdio.h"
#include "string.h"
#include "sys/wait.h"
#include "unistd.h"

#define MAX_LINE_LENGTH  256
#define MAX_PROGRAM_ARGS 8

const char *PATH = "/bin/";
char CWD[MAX_LINE_LENGTH] = {0};
char input_line[MAX_LINE_LENGTH] = {0};
char program_name[MAX_LINE_LENGTH] = {0};
char *program_args[MAX_PROGRAM_ARGS] = {0};

static void
sh_help()
{
    printf("Usage sh:\n");
    printf("Latte interactive shell\n");
}

static int
sh_init()
{
    // Initial current working directory
    strcpy(CWD, "/");

    return 0;
}

static int
sh_reset_input()
{
    memset(input_line, 0, MAX_LINE_LENGTH);
    memset(program_name, 0, MAX_LINE_LENGTH);
    memset(program_args, 0, MAX_PROGRAM_ARGS);

    return 0;
}

static inline void
sh_prompt()
{
    printf("sh:%s> ", CWD);
}

static int
sh_cd(const char *path)
{
    strcpy(CWD, path);
    sh_prompt();

    return 0;
}

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

static char *
parse_program_name(char *line)
{
    char *prog_start = line;
    while (*line && *line != ' ') {
        line++;
    }

    while (*line && *line == ' ') {
        *line = 0;
        line++;
    }

    sprintf(program_name, "%s%s", PATH, prog_start);
    return line;
}

static char *
parse_arg(char *line, char **arg)
{
    if (!*line) {
        return NULL;
    }

    char *arg_start = line;
    while (*line && *line != ' ') {
        line++;
    }

    while (*line && *line == ' ') {
        *line = 0;
        line++;
    }

    *arg = arg_start;
    return line;
}

static int
parse_program_args(char *line)
{
    int i;
    for (i = 0; i < 8; i++) {
        char *arg = NULL;
        line = parse_arg(line, &arg);
        if (!arg) {
            break;
        }

        program_args[i] = arg;
    }

    return i;
}

int
main(int argc, char **argv)
{
    if (argc > 0) {
        sh_help();
        return 0;
    }

    sh_init();

    while (1) {
        sh_prompt();

        sh_reset_input();

        int line_length = readline(input_line, 256);
        if (line_length == 0) {
            continue; // User pressed enter, but no other input
        }

        char *arg_start = parse_program_name(input_line);
        int argc = parse_program_args(arg_start);

        if (strcmp(program_name, "cd") == 0) {
            sh_cd(program_args[0]);
            continue;
        }

        pid_t pid = fork();
        if ((int)pid < 0) {
            printf("sh ERROR: Failed to fork()");
        } else if (pid == 0) {
            // Child
            execv(program_name, program_args);
        } else {
            // Parent
            int status_code;
            pid_t child_pid = wait(&status_code);

            if (status_code != 0) {
                printf("Command \'%s\' not found\n", program_name);
            }
        }
    }
}
