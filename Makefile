version = 0.1
builddir = ./build

CC = i686-elf-gcc
AS = nasm

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASFLAGS =  -f elf

ifdef $(DEBUG)
CFLAGS += -g
ASFLAGS += -g
endif

INCLUDES = -Isys/include

all: kernel libs bins

include sys/Makefile

include lib/Makefile

include bin/Makefile

clean:
	rm -r $(builddir)

