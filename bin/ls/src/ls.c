#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "dirent.h"

static void
print_help()
{
    printf("Usage: /bin/ls {DIRECTORY}\n");
    printf("List information about files and directories\n");
}

static void
print_dir(char *dir_path)
{
    DIR *dir = opendir(dir_path);
    if (!dir) {
	printf("\"%s\": No such file or directory\n", dir_path);
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
}

int
main(int argc, char **argv)
{
    if (argc < 1) {
	print_help();
	_exit(EXIT_FAILURE);
    }

    char *dir_path = argv[0];
    print_dir(dir_path);

    return 0;
}
