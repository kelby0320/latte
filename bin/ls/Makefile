version = 0.1
builddir = ../../build

CC = i686-elf-gcc

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

ifdef $(DEBUG)
CFLAGS += -g
endif

INCLUDES = -I../../include

all: ls
	$(CC) $(CFLAGS) -nostdlib -fpic -T ./linker.ld -o $(builddir)/bin/ls/ls $(shell find $(builddir)/bin/ls -name *.o) $(builddir)/lib/libc/libc.a

include src/Makefile
