CAT = ./cat

LIBC = ../../lib/libc/libc.a

CC = i686-elf-gcc

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra

C_OBJECTS = ./src/cat.o

INCLUDES=-I../../include

all: $(CAT)

$(CAT): $(C_OBJECTS)
	$(CC) -g -T ./linker.ld -o $(CAT) -ffreestanding -O0 -nostdlib -fpic $(C_OBJECTS) $(LIBC)

$(filter %.o,$(C_OBJECTS)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	-rm -r $(C_OBJECTS)
	-rm $(CAT)
