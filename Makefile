CC = i686-elf-gcc
AS = i686-elf-as

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra

OBJECTS = ./build/boot/boot.S.o \
	./build/sys/gdt/gdt.o \
	./build/sys/gdt/gdt.S.o \
	./build/sys/kernel.o \
	./build/sys/libk/kheap.o \
	./build/sys/libk/libk.o \
	./build/sys/libk/memory.o \
	./build/sys/libk/string.o \
	./build/sys/mem/heap.o

TEST_OBJECTS = ./build/test/test.o \
	./build/test/sys/libk/memory.o \
	./build/test/sys/libk/test_memory.o \
	./build/test/sys/libk/string.o \
	./build/test/sys/libk/test_string.o \
	./build/test/sys/mem/heap.o \
	./build/test/sys/mem/test_heap.o

TEST_EXECUTABLES = ./build/test/sys/libk/test_memory \
	./build/test/sys/libk/test_string \
	./build/test/sys/mem/test_heap

INCLUDES = -I./sys

TEST_INCLUDES = -I./sys -I./test

all: ./build/boot/latte.elf

build_dirs:
	mkdir -p ./build/boot
	mkdir -p ./build/sys/gdt
	mkdir -p ./build/sys/libk
	mkdir -p ./build/sys/mem
	mkdir -p ./build/test/sys/libk
	mkdir -p ./build/test/sys/mem

./build/boot/latte.elf: build_dirs $(OBJECTS)
	$(CC) -T ./sys/linker.ld -o ./build/boot/latte.elf -ffreestanding -O0 -nostdlib $(OBJECTS) -lgcc

./build/boot/boot.S.o: ./boot/boot.S
	$(AS) ./boot/boot.S -o ./build/boot/boot.S.o

./build/sys/gdt/gdt.o: ./sys/gdt/gdt.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/gdt/gdt.c -o ./build/sys/gdt/gdt.o

./build/sys/gdt/gdt.S.o: ./sys/gdt/gdt.S
	$(AS) ./sys/gdt/gdt.S -o ./build/sys/gdt/gdt.S.o

./build/sys/kernel.o: ./sys/kernel.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/kernel.c -o ./build/sys/kernel.o

./build/sys/libk/kheap.o: ./sys/libk/kheap.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/libk/kheap.c -o ./build/sys/libk/kheap.o

./build/sys/libk/libk.o: ./sys/libk/libk.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/libk/libk.c -o ./build/sys/libk/libk.o

./build/sys/libk/memory.o: ./sys/libk/memory.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/libk/memory.c -o ./build/sys/libk/memory.o

./build/sys/libk/string.o: ./sys/libk/string.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/libk/string.c -o ./build/sys/libk/string.o

./build/sys/mem/heap.o: ./sys/mem/heap.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/mem/heap.c -o ./build/sys/mem/heap.o

test: build_dirs $(TEST_OBJECTS) $(TEST_EXECUTABLES)
	./build/test/sys/libk/test_memory
	./build/test/sys/libk/test_string
	./build/test/sys/mem/test_heap

./build/test/test.o: ./test/test.c
	gcc -g $(TEST_INCLUDES) -c ./test/test.c -o ./build/test/test.o

./build/test/sys/libk/memory.o: ./sys/libk/memory.c
	gcc -g $(TEST_INCLUDES) -c ./sys/libk/memory.c -o ./build/test/sys/libk/memory.o

./build/test/sys/libk/test_memory.o: ./test/sys/libk/test_memory.c
	gcc -g $(TEST_INCLUDES) -c ./test/sys/libk/test_memory.c -o ./build/test/sys/libk/test_memory.o

./build/test/sys/libk/test_memory: $(TEST_OBJECTS)
	gcc -g ./build/test/test.o ./build/test/sys/libk/memory.o ./build/test/sys/libk/test_memory.o -o ./build/test/sys/libk/test_memory

./build/test/sys/libk/string.o: ./sys/libk/string.c
	gcc -g $(TEST_INCLUDES) -c ./sys/libk/string.c -o ./build/test/sys/libk/string.o

./build/test/sys/libk/test_string.o: ./test/sys/libk/test_string.c
	gcc -g $(TEST_INCLUDES) -c ./test/sys/libk/test_string.c -o ./build/test/sys/libk/test_string.o

./build/test/sys/libk/test_string: $(TEST_OBJECTS)
	gcc -g ./build/test/test.o ./build/test/sys/libk/string.o ./build/test/sys/libk/test_string.o -o ./build/test/sys/libk/test_string

./build/test/sys/mem/heap.o: ./sys/mem/heap.c
	gcc -g $(TEST_INCLUDES) -c ./sys/mem/heap.c -o ./build/test/sys/mem/heap.o

./build/test/sys/mem/test_heap.o: ./test/sys/mem/test_heap.c
	gcc -g $(TEST_INCLUDES) -c ./test/sys/mem/test_heap.c -o ./build/test/sys/mem/test_heap.o

./build/test/sys/mem/test_heap: $(TEST_OBJECTS)
	gcc -g ./build/test/test.o ./build/test/sys/mem/heap.o ./build/test/sys/mem/test_heap.o -o ./build/test/sys/mem/test_heap

clean:
	rm -r ./build
