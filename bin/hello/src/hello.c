#include "stdio.h"
#include "unistd.h"

int
main(int argc, char **argv)
{
    printf("Hello World!\n");

    char buf[128] = {0};
    int num_read = read(stdin, buf, 10);
    printf("num_read: %d\n", num_read);

    for (int i = 0; i < argc; i++) {
	printf("%s\n", argv[i]);
    }
    
    return 0;
}
