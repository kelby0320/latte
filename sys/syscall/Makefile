syscall_subdirs = process
syscall_subdirs_targets = $(foreach dir, $(syscall_subdirs), syscall_$(dir))
syscalldir = sys/syscall
syscall_builddir = $(builddir)/$(syscalldir)
syscall_objs = io.o mmap.o syscall.o
syscall_objfiles = $(foreach obj, $(syscall_objs), $(syscalldir)/$(obj))

syscall: syscall_mkdir $(syscall_objfiles) $(syscall_subdirs_targets)

syscall_mkdir:
	mkdir -p $(syscall_builddir)

$(filter %.o, $(syscall_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

include $(foreach dir, $(syscall_subdirs), sys/syscall/$(dir)/Makefile)
