libc_sys_subdirs = mmap
libc_sys_subdirs_targets = $(foreach dir, $(libc_sys_subdirs), libc_sys_$(dir))
libc_sysdir = src/sys
libc_sys_builddir = $(builddir)/lib/libc/$(libc_sysdir)
libc_sys_objs = wait.o
libc_sys_objfiles = $(foreach obj, $(libc_sys_objs), $(libc_sysdir)/$(obj))

libc_sys: libc_sys_mkdir $(libc_sys_objfiles) $(libc_sys_subdirs_targets)

libc_sys_mkdir:
	mkdir -p $(libc_sys_builddir)

$(filter %.o, $(libc_sys_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/lib/libc/$@ -c $<

include $(foreach dir, $(libc_sys_subdirs), $(libc_sysdir)/$(dir)/Makefile)
