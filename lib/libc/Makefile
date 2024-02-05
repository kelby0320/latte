LIBC = ./libc.a

CC = i686-elf-gcc
AS = i686-elf-as
AR = i686-elf-ar

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra
ASFLAGS = -g

C_OBJECTS = ./src/ctype.o \
	./src/init.o \
	./src/stdio.o \
	./src/stdlib.o \
	./src/string.o 

AS_OBJECTS = ./src/crt0.s.o 

INCLUDES=-I../../include

all: $(LIBC)

$(LIBC): $(C_OBJECTS) $(AS_OBJECTS)
	$(AR) rcs $(LIBC) $(C_OBJECTS) $(AS_OBJECTS)

$(filter %.o,$(C_OBJECTS)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(filter %.s.o,$(AS_OBJECTS)): %.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	-rm -rf $(C_OBJECTS) $(AS_OBJECTS)
	-rm $(LIBC)
