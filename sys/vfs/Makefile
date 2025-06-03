vfsdir = sys/vfs
vfs_builddir = $(builddir)/$(vfsdir)
vfs_objs = file_descriptor.o mountpoint.o vfs.o
vfs_objfiles = $(foreach obj, $(vfs_objs), $(vfsdir)/$(obj))

vfs: vfs_mkdir $(vfs_objfiles)

vfs_mkdir:
	mkdir -p $(vfs_builddir)

$(filter %.o, $(vfs_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
