libc_direntdir = src/dirent
libc_dirent_builddir = $(builddir)/lib/libc/$(libc_direntdir)
libc_dirent_objs = closedir.o opendir.o readdir.o
libc_dirent_objfiles = $(foreach obj, $(libc_dirent_objs), $(libc_direntdir)/$(obj))

libc_dirent: libc_dirent_mkdir $(libc_dirent_objfiles)

libc_dirent_mkdir:
	mkdir -p $(libc_dirent_builddir)

$(filter %.o, $(libc_dirent_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/lib/libc/$@ -c $<
