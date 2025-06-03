version = 0.1
builddir = ./build

CC = i686-elf-gcc
AS = nasm

CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra
ASFLAGS =  -f elf

ifdef DEBUG
CFLAGS += -g
ASFLAGS += -g
endif

INCLUDES = -Isys/include

all: kernel libs bins

libs:
	cd ./lib && $(MAKE) all

bins:
	cd ./bin && $(MAKE) all

clean:
	rm -r $(builddir)

img:
	./scripts/make_img.sh

include sys/Makefile


