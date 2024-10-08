MAJOR_VERSION = 0
MINOR_VERSION = 1
KERNEL = ./boot/latte-$(MAJOR_VERSION).$(MINOR_VERSION)

CC = i686-elf-gcc
AS = nasm

CFLAGS = -g -std=gnu99 -ffreestanding -O0 -Wall -Wextra
ASFLAGS = -f elf -g

INCLUDES = -I./sys
TEST_INCLUDES = -I./sys -I./test

C_OBJECTS = ./sys/block/block.o \
	./sys/block/buffered_reader.o \
	./sys/block/disk.o \
	./sys/block/partition.o \
	./sys/boot/boot.o \
	./sys/boot/multiboot2.o \
	./sys/cpu/cpu.o \
	./sys/dev/bus.o \
	./sys/dev/device.o\
	./sys/dev/input/input_device.o \
	./sys/dev/input/keycode.o \
	./sys/dev/platform/platform_device.o \
	./sys/drivers/driver.o \
	./sys/drivers/platform/platform_driver.o \
	./sys/drivers/platform/ata/ata.o \
	./sys/drivers/platform/ata/ata_io.o \
	./sys/drivers/platform/console/console.o \
	./sys/drivers/platform/console/console_read.o \
	./sys/drivers/platform/console/console_write.o \
	./sys/drivers/platform/devfs/devfs.o \
	./sys/drivers/platform/kbd/kbd.o \
	./sys/drivers/platform/kbd/ps2.o \
	./sys/drivers/platform/vga/vga.o \
	./sys/fs/fs.o \
	./sys/fs/path.o \
	./sys/fs/devfs/devfs.o \
	./sys/fs/ext2/block_iter.o \
	./sys/fs/ext2/dir.o \
	./sys/fs/ext2/dir_iter.o \
	./sys/fs/ext2/ext2.o \
	./sys/fs/ext2/inode.o \
	./sys/fs/fat32/fat32.o \
	./sys/gdt/gdt.o \
	./sys/gdt/tss.o \
	./sys/irq/idt.o \
	./sys/irq/irq.o \
	./sys/irq/isr.o \
	./sys/kernel/main.o \
	./sys/kernel/msgbuf.o \
	./sys/kernel/term.o \
	./sys/libk/alloc.o \
	./sys/libk/list.o \
	./sys/libk/memory.o \
	./sys/libk/print.o \
	./sys/libk/queue.o \
	./sys/libk/slab.o \
	./sys/libk/string.o \
	./sys/mm/paging/page_dir.o \
	./sys/mm/paging/page_tbl.o \
	./sys/mm/paging/paging.o \
	./sys/mm/buddy.o \
	./sys/mm/kalloc.o \
	./sys/mm/vm.o \
	./sys/proc/mgmt/exec.o \
	./sys/proc/mgmt/exit.o \
	./sys/proc/mgmt/fork.o \
	./sys/proc/mgmt/wait.o \
	./sys/proc/elf.o \
	./sys/proc/fd.o \
	./sys/proc/ld.o \
	./sys/proc/mmap.o \
	./sys/proc/process.o \
	./sys/sched/sched.o \
	./sys/syscall/io.o \
	./sys/syscall/mmap.o \
	./sys/syscall/process/exec.o \
	./sys/syscall/process/exit.o \
	./sys/syscall/process/fork.o \
	./sys/syscall/process/wait.o \
	./sys/syscall/syscall.o \
	./sys/thread/thread.o \
	./sys/thread/userio.o \
	./sys/vfs/file_descriptor.o \
	./sys/vfs/mountpoint.o \
	./sys/vfs/vfs.o \

AS_OBJECTS = ./sys/boot/boot.asm.o \
	./sys/boot/header.asm.o \
	./sys/cpu/port.asm.o \
	./sys/gdt/gdt.asm.o \
	./sys/gdt/tss.asm.o \
	./sys/irq/idt.asm.o \
	./sys/irq/irq.asm.o \
	./sys/mm/paging/paging.asm.o \
	./sys/thread/thread.asm.o

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
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(filter %.asm.o,$(AS_OBJECTS)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<

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

./test/sys/libk/alloc.o: ./sys/libk/alloc.c
	gcc -g $(TEST_INCLUDES) -c ./sys/libk/alloc.c -o ./test/sys/libk/alloc.o

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

bin: libs
	cd ./bin/cat && $(MAKE) all
	cd ./bin/hello && $(MAKE) all
	cd ./bin/init && $(MAKE) all
	cd ./bin/ls && $(MAKE) all
	cd ./bin/sh && $(MAKE) all

clean_libs:
	cd ./lib/libc && $(MAKE) clean

clean_bin:
	cd ./bin/sh && $(MAKE) clean
	cd ./bin/ls && $(MAKE) clean
	cd ./bin/init && $(MAKE) clean
	cd ./bin/hello && $(MAKE) clean
	cd ./bin/cat ** $(MAKE) clean

clean: clean_libs clean_bin
	-rm -r $(C_OBJECTS) $(AS_OBJECTS)
	-rm -r $(TEST_OBJECTS) $(TEST_EXECUTABLES)
	-rm $(KERNEL)
