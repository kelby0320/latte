libc_stringdir = src/string
libc_string_builddir = $(builddir)/lib/libc/$(libc_stringdir)
libc_string_objs = string.o
libc_string_objfiles = $(foreach obj, $(libc_string_objs), $(libc_stringdir)/$(obj))

libc_string: libc_string_mkdir $(libc_string_objfiles)

libc_string_mkdir:
	mkdir -p $(libc_string_builddir)

$(filter %.o, $(libc_string_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/lib/libc/$@ -c $<
