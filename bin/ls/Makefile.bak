LS = ./ls

LIBC = ../../lib/libc/libc.a

CC = i686-elf-gcc

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra

C_OBJECTS = ./src/ls.o

INCLUDES=-I../../include

all: $(LS)

$(LS): $(C_OBJECTS)
	$(CC) -g -T ./linker.ld -o $(LS) -ffreestanding -O0 -nostdlib -fpic $(C_OBJECTS) $(LIBC)

$(filter %.o,$(C_OBJECTS)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	-rm -r $(C_OBJECTS)
	-rm $(LS)
