version = 0.1
builddir = ../../build

CC = i686-elf-gcc

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

ifdef $(DEBUG)
CFLAGS += -g
endif

INCLUDES = -I../../include

all: cat
	$(CC) $(CFLAGS) -nostdlib -fpic -T ./linker.ld -o $(builddir)/bin/cat/cat $(shell find $(builddir)/bin/cat -name *.o) $(builddir)/lib/libc/libc.a

include src/Makefile
