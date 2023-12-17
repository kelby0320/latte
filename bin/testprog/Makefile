CC = i686-elf-gcc

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra

INCLUDES=-I../../include

BUILDDIR = ../../build
LIBDIR = $(BUILDDIR)/lib

OBJECTS = $(BUILDDIR)/bin/testprog.o

all: testprog

testprog: $(OBJECTS)
	$(CC) -g -T ./linker.ld -o $(BUILDDIR)/bin/testprog -ffreestanding -O0 -nostdlib -fpic $(OBJECTS) $(LIBDIR)/libc.a

$(BUILDDIR)/bin/testprog.o: ./src/main.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/main.c -o $(BUILDDIR)/bin/testprog.o

clean:
	rm -rf $(OBJECTS)
	rm -f $(BUILDDIR)/bin/testprog
