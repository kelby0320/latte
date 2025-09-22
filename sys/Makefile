kernel_subdirs = block boot cpu dev driver fs gdt init irq libk mm process \
	sched syscall thread vfs

kernel: $(kernel_subdirs)
	$(CC) $(LDFLAGS) -o $(builddir)/sys/latte-$(version) $(shell find build/sys -name *.o)

include $(foreach dir, $(kernel_subdirs), sys/$(dir)/Makefile)
