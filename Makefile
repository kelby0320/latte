CC = i686-elf-gcc
AS = nasm

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra
ASFLAGS = -f elf -g

OBJECTS = ./build/boot/boot.asm.o \
	./build/sys/dev/disk/ata/ata.o \
	./build/sys/dev/disk/buffer/bufferedreader.o \
	./build/sys/dev/disk/disk.o \
	./build/sys/dev/term/term.o \
	./build/sys/fs/fs.o \
	./build/sys/fs/path.o \
	./build/sys/fs/ext2/dir.o \
	./build/sys/fs/ext2/ext2.o \
	./build/sys/fs/ext2/inode.o \
	./build/sys/fs/fat32/fat32.o \
	./build/sys/gdt/gdt.asm.o \
	./build/sys/gdt/gdt.o \
	./build/sys/gdt/tss.asm.o \
	./build/sys/gdt/tss.o \
	./build/sys/irq/idt.asm.o \
	./build/sys/irq/idt.o \
	./build/sys/irq/irq.asm.o \
	./build/sys/irq/irq.o \
	./build/sys/kernel.o \
	./build/sys/libk/kheap.o \
	./build/sys/libk/libk.o \
	./build/sys/libk/memory.o \
	./build/sys/libk/string.o \
	./build/sys/mem/heap.o \
	./build/sys/mem/vm.asm.o \
	./build/sys/mem/vm.o \
	./build/sys/port/io.asm.o \
	./build/sys/task/elf.o \
	./build/sys/task/loader.o \
	./build/sys/task/process.o \
	./build/sys/task/sched.o \
	./build/sys/task/task.asm.o \
	./build/sys/task/task.o

TEST_OBJECTS = ./build/test/test.o \
	./build/test/sys/fs/path.o \
	./build/test/sys/fs/test_path.o \
	./build/test/sys/libk/memory.o \
	./build/test/sys/libk/test_memory.o \
	./build/test/sys/libk/string.o \
	./build/test/sys/libk/test_string.o \
	./build/test/sys/mem/heap.o \
	./build/test/sys/mem/test_heap.o

TEST_EXECUTABLES = ./build/test/sys/fs/test_path \
	./build/test/sys/libk/test_memory \
	./build/test/sys/libk/test_string \
	./build/test/sys/mem/test_heap

INCLUDES = -I./sys

TEST_INCLUDES = -I./sys -I./test

all: ./build/boot/latte.elf

build_dirs:
	mkdir -p ./build/boot
	mkdir -p ./build/sys/dev/disk/ata
	mkdir -p ./build/sys/dev/disk/buffer
	mkdir -p ./build/sys/dev/term
	mkdir -p ./build/sys/fs
	mkdir -p ./build/sys/fs/ext2
	mkdir -p ./build/sys/fs/fat32
	mkdir -p ./build/sys/gdt
	mkdir -p ./build/sys/irq
	mkdir -p ./build/sys/libk
	mkdir -p ./build/sys/mem
	mkdir -p ./build/sys/port
	mkdir -p ./build/sys/task
	mkdir -p ./build/test/sys/fs
	mkdir -p ./build/test/sys/libk
	mkdir -p ./build/test/sys/mem

./build/boot/latte.elf: build_dirs $(OBJECTS)
	$(CC) -T ./sys/linker.ld -o ./build/boot/latte.elf -ffreestanding -O0 -nostdlib $(OBJECTS) -lgcc

./build/boot/boot.asm.o: ./boot/boot.asm
	$(AS) $(ASFLAGS) ./boot/boot.asm -o ./build/boot/boot.asm.o

./build/sys/dev/disk/ata/ata.o: ./sys/dev/disk/ata/ata.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/dev/disk/ata/ata.c -o ./build/sys/dev/disk/ata/ata.o

./build/sys/dev/disk/buffer/bufferedreader.o: ./sys/dev/disk/buffer/bufferedreader.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/dev/disk/buffer/bufferedreader.c -o ./build/sys/dev/disk/buffer/bufferedreader.o

./build/sys/dev/disk/disk.o: ./sys/dev/disk/disk.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/dev/disk/disk.c -o ./build/sys/dev/disk/disk.o

./build/sys/dev/term/term.o: ./sys/dev/term/term.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/dev/term/term.c -o ./build/sys/dev/term/term.o

./build/sys/fs/fs.o: ./sys/fs/fs.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/fs/fs.c -o ./build/sys/fs/fs.o

./build/sys/fs/path.o: ./sys/fs/path.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/fs/path.c -o ./build/sys/fs/path.o

./build/sys/fs/ext2/dir.o: ./sys/fs/ext2/dir.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/fs/ext2/dir.c -o ./build/sys/fs/ext2/dir.o

./build/sys/fs/ext2/ext2.o: ./sys/fs/ext2/ext2.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/fs/ext2/ext2.c -o ./build/sys/fs/ext2/ext2.o

./build/sys/fs/ext2/inode.o: ./sys/fs/ext2/inode.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/fs/ext2/inode.c -o ./build/sys/fs/ext2/inode.o

./build/sys/fs/fat32/fat32.o: ./sys/fs/fat32/fat32.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/fs/fat32/fat32.c -o ./build/sys/fs/fat32/fat32.o

./build/sys/gdt/gdt.asm.o: ./sys/gdt/gdt.asm
	$(AS) $(ASFLAGS) ./sys/gdt/gdt.asm -o ./build/sys/gdt/gdt.asm.o

./build/sys/gdt/gdt.o: ./sys/gdt/gdt.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/gdt/gdt.c -o ./build/sys/gdt/gdt.o

./build/sys/gdt/tss.asm.o: ./sys/gdt/tss.asm
	$(AS) $(ASFLAGS) ./sys/gdt/tss.asm -o ./build/sys/gdt/tss.asm.o

./build/sys/gdt/tss.o: ./sys/gdt/tss.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/gdt/tss.c -o ./build/sys/gdt/tss.o

./build/sys/irq/idt.asm.o: ./sys/irq/idt.asm
	$(AS) $(ASFLAGS) ./sys/irq/idt.asm -o ./build/sys/irq/idt.asm.o

./build/sys/irq/idt.o: ./sys/irq/idt.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/irq/idt.c -o ./build/sys/irq/idt.o

./build/sys/irq/irq.asm.o: ./sys/irq/irq.asm
	$(AS) $(ASFLAGS) ./sys/irq/irq.asm -o ./build/sys/irq/irq.asm.o

./build/sys/irq/irq.o: ./sys/irq/irq.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/irq/irq.c -o ./build/sys/irq/irq.o

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

./build/sys/mem/vm.asm.o: ./sys/mem/vm.asm
	$(AS) $(ASFLAGS) ./sys/mem/vm.asm -o ./build/sys/mem/vm.asm.o

./build/sys/mem/vm.o: ./sys/mem/vm.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/mem/vm.c -o ./build/sys/mem/vm.o

./build/sys/port/io.asm.o: ./sys/port/io.asm
	$(AS) $(ASFLAGS) ./sys/port/io.asm -o ./build/sys/port/io.asm.o

./build/sys/task/elf.o: ./sys/task/elf.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/task/elf.c -o ./build/sys/task/elf.o

./build/sys/task/loader.o: ./sys/task/loader.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/task/loader.c -o ./build/sys/task/loader.o

./build/sys/task/process.o: ./sys/task/process.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/task/process.c -o ./build/sys/task/process.o

./build/sys/task/sched.o: ./sys/task/sched.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/task/sched.c -o ./build/sys/task/sched.o

./build/sys/task/task.asm.o: ./sys/task/task.asm
	$(AS) $(ASFLAGS) ./sys/task/task.asm -o ./build/sys/task/task.asm.o

./build/sys/task/task.o: ./sys/task/task.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./sys/task/task.c -o ./build/sys/task/task.o

test: build_dirs $(TEST_OBJECTS) $(TEST_EXECUTABLES)
	./build/test/sys/fs/test_path
	./build/test/sys/libk/test_memory
	./build/test/sys/libk/test_string
	./build/test/sys/mem/test_heap

./build/test/test.o: ./test/test.c
	gcc -g $(TEST_INCLUDES) -c ./test/test.c -o ./build/test/test.o
	
./build/test/sys/fs/path.o: ./sys/fs/path.c
	gcc -g $(TEST_INCLUDES) -c ./sys/fs/path.c -o ./build/test/sys/fs/path.o

./build/test/sys/fs/test_path.o: ./test/sys/fs/test_path.c
	gcc -g $(TEST_INCLUDES) -c ./test/sys/fs/test_path.c -o ./build/test/sys/fs/test_path.o

./build/test/sys/fs/test_path: $(TEST_OBJECTS)
	gcc -g ./build/test/test.o  ./build/test/sys/fs/path.o ./build/test/sys/fs/test_path.o -o ./build/test/sys/fs/test_path

./build/test/sys/libk/kheap.o: ./sys/libk/kheap.c
	gcc -g $(TEST_INCLUDES) -c ./sys/libk/kheap.c -o ./build/test/sys/libk/kheap.o

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
