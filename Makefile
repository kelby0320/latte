MAJOR_VERSION = 0
MINOR_VERSION = 1
KERNEL = ./boot/latte-$(MAJOR_VERSION).$(MINOR_VERSION)

CC = i686-elf-gcc
AS = i686-elf-as

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra
ASFLAGS = -g

INCLUDES = -I./sys
TEST_INCLUDES = -I./sys -I./test

C_OBJECTS = ./sys/boot/multiboot2.o \
	./sys/bus/ata/ata.o \
	./sys/bus/virtual/dev/dev.o \
	./sys/bus/virtual/devfs/devfs.o \
	./sys/bus/virtual/virtual.o \
	./sys/bus/bus.o \
	./sys/bus/mass_storage.o \
	./sys/dev/block/block.o \
	./sys/dev/term/term.o \
	./sys/dev/vga/vga.o \
	./sys/dev/device.o \
	./sys/fs/bufferedreader.o \
	./sys/fs/fs.o \
	./sys/fs/path.o \
	./sys/fs/devfs/devfs.o \
	./sys/fs/ext2/dir.o \
	./sys/fs/ext2/ext2.o \
	./sys/fs/ext2/inode.o \
	./sys/fs/fat32/fat32.o \
	./sys/gdt/gdt.o \
	./sys/gdt/tss.o \
	./sys/irq/idt.o \
	./sys/irq/irq.o \
	./sys/irq/isr.o \
	./sys/libk/kheap.o \
	./sys/libk/memory.o \
	./sys/libk/print.o \
	./sys/libk/string.o \
	./sys/mem/heap.o \
	./sys/mem/vm.o \
	./sys/syscall/io.o \
	./sys/syscall/mmap.o \
	./sys/syscall/syscall.o \
	./sys/task/elf.o \
	./sys/task/loader.o \
	./sys/task/process.o \
	./sys/task/sched.o \
	./sys/task/task.o \
	./sys/vfs/file_descriptor.o \
	./sys/vfs/mountpoint.o \
	./sys/vfs/vfs.o \
	./sys/kernel.o \
	./sys/msgbuf.o

AS_OBJECTS = ./sys/boot/boot.s.o \
	./sys/gdt/gdt.s.o \
	./sys/gdt/tss.s.o \
	./sys/irq/idt.s.o \
	./sys/irq/irq.s.o \
	./sys/mem/vm.s.o \
	./sys/port/io.s.o \
	./sys/task/task.s.o

TEST_OBJECTS = ./test/test.o \
	./test/sys/fs/path.o \
	./test/sys/fs/test_path.o \
	./test/sys/libk/memory.o \
	./test/sys/libk/test_memory.o \
	./test/sys/libk/string.o \
	./test/sys/libk/test_string.o \
	./test/sys/mem/heap.o \
	./test/sys/mem/test_heap.o

TEST_EXECUTABLES = ./test/sys/fs/test_path \
	./test/sys/libk/test_memory \
	./test/sys/libk/test_string \
	./test/sys/mem/test_heap

all: $(KERNEL) libs bin

$(KERNEL): $(C_OBJECTS) $(AS_OBJECTS)
	$(CC) -T ./sys/linker.ld -o $(KERNEL) -ffreestanding -O0 -nostdlib $(C_OBJECTS) $(AS_OBJECTS) -lgcc

$(filter %.o,$(C_OBJECTS)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(filter %.s.o,$(AS_OBJECTS)): %.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

test: $(TEST_OBJECTS) $(TEST_EXECUTABLES)
	./test/sys/fs/test_path
	./test/sys/libk/test_memory
	./test/sys/libk/test_string
	./test/sys/mem/test_heap

./test/test.o: ./test/test.c
	gcc -g $(TEST_INCLUDES) -c ./test/test.c -o ./test/test.o
	
./test/sys/fs/path.o: ./sys/fs/path.c
	gcc -g $(TEST_INCLUDES) -c ./sys/fs/path.c -o ./test/sys/fs/path.o

./test/sys/fs/test_path.o: ./test/sys/fs/test_path.c
	gcc -g $(TEST_INCLUDES) -c ./test/sys/fs/test_path.c -o ./test/sys/fs/test_path.o

./test/sys/fs/test_path: $(TEST_OBJECTS)
	gcc -g ./test/test.o  ./test/sys/fs/path.o ./test/sys/fs/test_path.o -o ./test/sys/fs/test_path

./test/sys/libk/kheap.o: ./sys/libk/kheap.c
	gcc -g $(TEST_INCLUDES) -c ./sys/libk/kheap.c -o ./test/sys/libk/kheap.o

./test/sys/libk/memory.o: ./sys/libk/memory.c
	gcc -g $(TEST_INCLUDES) -c ./sys/libk/memory.c -o ./test/sys/libk/memory.o

./test/sys/libk/test_memory.o: ./test/sys/libk/test_memory.c
	gcc -g $(TEST_INCLUDES) -c ./test/sys/libk/test_memory.c -o ./test/sys/libk/test_memory.o

./test/sys/libk/test_memory: $(TEST_OBJECTS)
	gcc -g ./test/test.o ./test/sys/libk/memory.o ./test/sys/libk/test_memory.o -o ./test/sys/libk/test_memory

./test/sys/libk/string.o: ./sys/libk/string.c
	gcc -g $(TEST_INCLUDES) -c ./sys/libk/string.c -o ./test/sys/libk/string.o

./test/sys/libk/test_string.o: ./test/sys/libk/test_string.c
	gcc -g $(TEST_INCLUDES) -c ./test/sys/libk/test_string.c -o ./test/sys/libk/test_string.o

./test/sys/libk/test_string: $(TEST_OBJECTS)
	gcc -g ./test/test.o ./test/sys/libk/string.o ./test/sys/libk/test_string.o -o ./test/sys/libk/test_string

./test/sys/mem/heap.o: ./sys/mem/heap.c
	gcc -g $(TEST_INCLUDES) -c ./sys/mem/heap.c -o ./test/sys/mem/heap.o

./test/sys/mem/test_heap.o: ./test/sys/mem/test_heap.c
	gcc -g $(TEST_INCLUDES) -c ./test/sys/mem/test_heap.c -o ./test/sys/mem/test_heap.o

./test/sys/mem/test_heap: $(TEST_OBJECTS)
	gcc -g ./test/test.o ./test/sys/mem/heap.o ./test/sys/mem/test_heap.o -o ./test/sys/mem/test_heap

libs:
	cd ./lib/libc && $(MAKE) all
	cd ./lib/liblatte && $(MAKE) all

bin: libs
	cd ./bin/testprog && $(MAKE) all

clean_libs:
	cd ./lib/libc && $(MAKE) clean
	cd ./lib/liblatte && $(MAKE) clean

clean_bin:
	cd ./bin/testprog && $(MAKE) clean

clean: clean_libs clean_bin
	-rm -r $(C_OBJECTS) $(AS_OBJECTS)
	-rm -r $(TEST_OBJECTS) $(TEST_EXECUTABLES)
	-rm $(KERNEL)