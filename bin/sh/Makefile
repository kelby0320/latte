version = 0.1
builddir = ../../build

CC = i686-elf-gcc

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

ifdef $(DEBUG)
CFLAGS += -g
endif

INCLUDES = -I../../include

all: sh
	$(CC) $(CFLAGS) -nostdlib -fpic -T ./linker.ld -o $(builddir)/bin/sh/sh $(shell find $(builddir)/bin/sh -name *.o) $(builddir)/lib/libc/libc.a

include src/Makefile
