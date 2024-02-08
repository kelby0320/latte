LIBLATTE = ./liblatte.a

CC = i686-elf-gcc
AS = nasm
AR = i686-elf-ar

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra
ASFLAGS = -f elf -g

C_OBJECTS = ./src/io.o \
	./src/mmap.o

AS_OBJECTS = ./src/syscall.asm.o

INCLUDES=-I../../include

all: $(LIBLATTE)

$(LIBLATTE): $(C_OBJECTS) $(AS_OBJECTS)
	$(AR) rcs $(LIBLATTE) $(C_OBJECTS) $(AS_OBJECTS)

$(filter %.o,$(C_OBJECTS)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(filter %.asm.o,$(AS_OBJECTS)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	-rm -rf $(C_OBJECTS) $(AS_OBJECTS)
	-rm $(LIBLATTE)
