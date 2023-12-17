CC = i686-elf-gcc
AS = nasm
AR = i686-elf-ar

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra
ASFLAGS = -f elf -g

BUILDDIR = ../../build

OBJECTS = \
	$(BUILDDIR)/lib/start.asm.o \
	$(BUILDDIR)/lib/start.o

INCLUDES=-I../../include

all: $(BUILDDIR)/lib/libc.a

$(BUILDDIR)/lib/libc.a: $(OBJECTS)
	$(AR) rcs $(BUILDDIR)/lib/libc.a $(OBJECTS)

$(BUILDDIR)/lib/start.asm.o: ./src/start.asm
	$(AS) $(ASFLAGS) ./src/start.asm -o $(BUILDDIR)/lib/start.asm.o

$(BUILDDIR)/lib/start.o: ./src/start.o
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/start.c -o $(BUILDDIR)/lib/start.o

clean:
	rm -rf ${OBJECTS}
