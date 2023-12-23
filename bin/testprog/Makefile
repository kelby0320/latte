TESTPROG = ./testprog

LIBC = ../../lib/libc/libc.a

CC = i686-elf-gcc

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra

INCLUDES=-I../../include

C_OBJECTS = ./src/testprog.o

all: $(TESTPROG)

$(TESTPROG): $(C_OBJECTS)
	$(CC) -g -T ./linker.ld -o $(TESTPROG) -ffreestanding -O0 -nostdlib -fpic $(C_OBJECTS) $(LIBC)

$(filter %.o,$(C_OBJECTS)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	-rm -r $(C_OBJECTS)
	-rm $(TESTPROG)
