FILES = ./build/sys/kernel.o

FLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra

all: ./build/boot/latte.bin

./build/boot/latte.bin: ./build/boot/boot.o ./build/sys/kernel.o
	mkdir -p ./build/boot
	i686-elf-ld -g -relocatable $(FILES) -o ./build/sys/kernelfull.o
	i686-elf-gcc -T ./sys/linker.ld -o ./build/boot/latte.bin -ffreestanding -O0 -nostdlib ./build/boot/boot.o ./build/sys/kernel.o -lgcc

./build/boot/boot.o: ./boot/boot.S
	mkdir -p ./build/boot
	i686-elf-as ./boot/boot.S -o ./build/boot/boot.o

./build/sys/kernel.o: ./sys/kernel.c
	mkdir -p ./build/sys
	i686-elf-gcc ${FLAGS} -c ./sys/kernel.c -o ./build/sys/kernel.o

clean:
	rm -r ./build
