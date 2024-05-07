LIBC = ./libc.a

CC = i686-elf-gcc
AS = nasm
AR = i686-elf-ar

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra
ASFLAGS = -f elf -g

C_OBJECTS = ./src/stdio/printf.o \
	./src/stdio/stdio.o \
	./src/stdlib/free.o \
	./src/stdlib/malloc.o \
	./src/sys/mmap/mmap.o \
	./src/sys/mmap/munmap.o \
	./src/sys/wait.o \
	./src/ctype.o \
	./src/init.o \
	./src/string/string.o \
	./src/unistd/close.o \
	./src/unistd/exec.o \
	./src/unistd/exit.o \
	./src/unistd/fork.o \
	./src/unistd/open.o \
	./src/unistd/read.o \
	./src/unistd/write.o

AS_OBJECTS = ./src/crt0.asm.o \
	./src/syscall.asm.o

INCLUDES=-I../../include -I./src

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
