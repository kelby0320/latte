libkdir = sys/libk
libk_builddir = $(builddir)/$(libkdir)
libk_objs = alloc.o list.o memory.o print.o queue.o slab.o string.o
libk_objfiles = $(foreach obj, $(libk_objs), $(libkdir)/$(obj))

libk: libk_mkdir $(libk_objfiles)

libk_mkdir:
	mkdir -p $(libk_builddir)

$(filter %.o, $(libk_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
