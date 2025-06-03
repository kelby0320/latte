version = 0.1
builddir = ../../build

CC = i686-elf-gcc
AS = nasm
AR = i686-elf-ar

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASFLAGS =  -f elf

ifdef $(DEBUG)
CFLAGS += -g
ASFLAGS += -g
endif

INCLUDES = -I../../include -I./src

all: libc
	$(AR) rcs $(builddir)/lib/libc/libc.a $(shell find $(builddir)/lib/libc -name *.o)

include src/Makefile
