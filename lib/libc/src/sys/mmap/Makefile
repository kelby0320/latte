libc_sys_mmapdir = src/sys/mmap
libc_sys_mmap_builddir = $(builddir)/lib/libc/$(libc_sys_mmapdir)
libc_sys_mmap_objs = mmap.o munmap.o
libc_sys_mmap_objfiles = $(foreach obj, $(libc_sys_mmap_objs), $(libc_sys_mmapdir)/$(obj))

libc_sys_mmap: libc_sys_mmap_mkdir $(libc_sys_mmap_objfiles)

libc_sys_mmap_mkdir:
	mkdir -p $(libc_sys_mmap_builddir)

$(filter %.o, $(libc_sys_mmap_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/lib/libc/$@ -c $<
