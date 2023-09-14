FILES = ./build/boot/boot.o \
	./build/sys/kernel.o \
	./build/sys/gdt/gdt.o \
	./build/sys/gdt/gdt.S.o \
	./build/sys/libk/string.o

INCLUDES = -I./sys \
	-I./sys/gdt \
	-I ./sys/libk

FLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra

all: ./build/boot/latte.elf

./build/boot/latte.elf: $(FILES)
	mkdir -p ./build/boot
	i686-elf-gcc -T ./sys/linker.ld -o ./build/boot/latte.elf -ffreestanding -O0 -nostdlib $(FILES) -lgcc

./build/boot/boot.o: ./boot/boot.S
	mkdir -p ./build/boot
	i686-elf-as ./boot/boot.S -o ./build/boot/boot.o

./build/sys/kernel.o: ./sys/kernel.c
	mkdir -p ./build/sys
	i686-elf-gcc $(FLAGS) $(INCLUDES) -c ./sys/kernel.c -o ./build/sys/kernel.o

./build/sys/gdt/gdt.o: ./sys/gdt/gdt.c
	mkdir -p ./build/sys/gdt
	i686-elf-gcc $(FLAGS) $(INCLUDES) -c ./sys/gdt/gdt.c -o ./build/sys/gdt/gdt.o

./build/sys/gdt/gdt.S.o: ./sys/gdt/gdt.S
	mkdir -p ./build/sys/gdt
	i686-elf-as ./sys/gdt/gdt.S -o ./build/sys/gdt/gdt.S.o

./build/sys/libk/string.o: ./sys/libk/string.c
	mkdir -p ./build/sys/libk
	i686-elf-gcc $(FLAGS) $(INCLUDES) -c ./sys/libk/string.c -o ./build/sys/libk/string.o

clean:
	rm -r ./build
