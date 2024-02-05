HELLO = ./hello

LIBC = ../../lib/libc/libc.a
LIBLATTE = ../../lib/liblatte/liblatte.a

CC = i686-elf-gcc

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra

C_OBJECTS = ./src/hello.o

INCLUDES=-I../../include

all: $(HELLO)

$(HELLO): $(C_OBJECTS)
	$(CC) -g -T ./linker.ld -o $(HELLO) -ffreestanding -O0 -nostdlib -fpic $(C_OBJECTS) $(LIBC) $(LIBLATTE)

$(filter %.o,$(C_OBJECTS)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	-rm -r $(C_OBJECTS)
	-rm $(HELLO)
