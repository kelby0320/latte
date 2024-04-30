LIBC = ./libc.a

CC = i686-elf-gcc
AS = nasm
AR = i686-elf-ar

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra
ASFLAGS = -f elf -g

C_OBJECTS = ./src/sys/mmap.o \
	./src/ctype.o \
	./src/init.o \
	./src/stdio.o \
	./src/stdlib.o \
	./src/string.o \
	./src/unistd.o

AS_OBJECTS = ./src/crt0.asm.o \
	./src/syscall.asm.o

INCLUDES=-I../../include

all: $(LIBC)

$(LIBC): $(C_OBJECTS) $(AS_OBJECTS)
	$(AR) rcs $(LIBC) $(C_OBJECTS) $(AS_OBJECTS)

$(filter %.o,$(C_OBJECTS)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(filter %.asm.o,$(AS_OBJECTS)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	-rm -rf $(C_OBJECTS) $(AS_OBJECTS)
	-rm $(LIBC)
