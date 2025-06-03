libc_unistddir = src/unistd
libc_unistd_builddir = $(builddir)/lib/libc/$(libc_unistddir)
libc_unistd_objs = close.o exec.o exit.o fork.o open.o read.o write.o
libc_unistd_objfiles = $(foreach obj, $(libc_unistd_objs), $(libc_unistddir)/$(obj))

libc_unistd: libc_unistd_mkdir $(libc_unistd_objfiles)

libc_unistd_mkdir:
	mkdir -p $(libc_unistd_builddir)

$(filter %.o, $(libc_unistd_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/lib/libc/$@ -c $<
