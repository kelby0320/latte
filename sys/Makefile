kernel_subdirs = block boot cpu dev driver fs gdt init irq libk mm process \
	sched syscall thread vfs

kernel: $(kernel_subdirs)
	$(CC) -T ./sys/linker.ld -o $(builddir)/sys/latte-$(version) -ffreestanding -O2 -nostdlib -lgcc $(shell find build/sys -name *.o)

include $(foreach dir, $(kernel_subdirs), sys/$(dir)/Makefile)
