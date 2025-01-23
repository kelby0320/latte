libc_stdlibdir = src/stdlib
libc_stdlib_builddir = $(builddir)/lib/libc/$(libc_stdlibdir)
libc_stdlib_objs = env.o free.o malloc.o
libc_stdlib_objfiles = $(foreach obj, $(libc_stdlib_objs), $(libc_stdlibdir)/$(obj))

libc_stdlib: libc_stdlib_mkdir $(libc_stdlib_objfiles)

libc_stdlib_mkdir:
	mkdir -p $(libc_stdlib_builddir)

$(filter %.o, $(libc_stdlib_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/lib/libc/$@ -c $<
