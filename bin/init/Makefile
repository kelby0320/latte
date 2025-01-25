version = 0.1
builddir = ../../build

CC = i686-elf-gcc

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

ifdef $(DEBUG)
CFLAGS += -g
endif

INCLUDES = -I../../include

all: init
	$(CC) $(CFLAGS) -nostdlib -fpic -T ./linker.ld -o $(builddir)/bin/init/init $(shell find $(builddir)/bin/init -name *.o) $(builddir)/lib/libc/libc.a

include src/Makefile
