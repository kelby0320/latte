libc_subdirs = dirent stdio stdlib string sys unistd
libc_subdirs_targets = $(foreach dir, $(libc_subdirs), libc_$(dir))
libcdir = src
libc_builddir = $(builddir)/lib/libc/$(libcdir)
libc_objs = ctype.o entry.o init.o
libc_objfiles = $(foreach obj, $(libc_objs), $(libcdir)/$(obj))
libc_asmobjs = crt0.asm.o syscall.asm.o
libc_asmobjfiles = $(foreach obj, $(libc_asmobjs), $(libcdir)/$(obj))

libc: libc_mkdir $(libc_objfiles) $(libc_asmobjfiles) $(libc_subdirs_targets)

libc_mkdir:
	mkdir -p $(libc_builddir)

$(filter %.o, $(libc_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/lib/libc/$@ -c $<

$(filter %.asm.o, $(libc_asmobjfiles)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) -o $(builddir)/lib/libc/$@ $<

include $(foreach dir, $(libc_subdirs), $(libcdir)/$(dir)/Makefile)
