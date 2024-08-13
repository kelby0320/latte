#include "stdio.h"
#include "unistd.h"

int
static readline(char *buf, size_t len)
{
    int num_read = 0;
    int total_read = 0;
    
    do {
	num_read = read(stdin, buf, len);
	
	if (num_read > 0) {
	    char final = buf[num_read - 1];
	    
	    buf += num_read;
	    len -= num_read;
	    total_read += num_read;
	    num_read = 0;

	    if (final == '\n') {
		buf[0] = 0; // Remove '\n'
		break;
	    }
	}
    } while (len > 0);

    return total_read;
}

int
main(int argc, char **argv)
{
    printf("Got arguments: ");

    for (int i = 0; i < argc; i++) {
	printf("%s ", argv[i]);
    }

    printf("\n");

    
    printf("What is your name? ");
    char buf[128] = {0};
    int num_read = readline(buf, 128);
    printf("\nHello %s\n", buf);

    return 0;
}
