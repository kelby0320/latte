fs_devfsdir = sys/fs/devfs
fs_devfs_builddir = $(builddir)/$(fs_devfsdir)
fs_devfs_objs = devfs.o
fs_devfs_objfiles = $(foreach obj, $(fs_devfs_objs), $(fs_devfsdir)/$(obj))

fs_devfs: fs_devfs_mkdir $(fs_devfs_objfiles)

fs_devfs_mkdir:
	mkdir -p $(fs_devfs_builddir)

$(filter %.o, $(fs_devfs_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

