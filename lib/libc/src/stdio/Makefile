libc_stdiodir = src/stdio
libc_stdio_builddir = $(builddir)/lib/libc/$(libc_stdiodir)
libc_stdio_objs = printf.o stdio.o
libc_stdio_objfiles = $(foreach obj, $(libc_stdio_objs), $(libc_stdiodir)/$(obj))

libc_stdio: libc_stdio_mkdir $(libc_stdio_objfiles)

libc_stdio_mkdir:
	mkdir -p $(libc_stdio_builddir)

$(filter %.o, $(libc_stdio_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/lib/libc/$@ -c $<
