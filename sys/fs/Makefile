fs_subdirs = devfs ext2 fat32
fs_subdirs_targets = $(foreach dir, $(fs_subdirs), fs_$(dir))
fsdir = sys/fs
fs_builddir = $(builddir)/$(fsdir)
fs_objs = fs.o path.o
fs_objfiles = $(foreach obj, $(fs_objs), $(fsdir)/$(obj))

fs: fs_mkdir $(fs_objfiles) $(fs_subdirs_targets)

fs_mkdir:
	mkdir -p $(fs_builddir)

$(filter %.o, $(fs_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

include $(foreach dir, $(fs_subdirs), sys/fs/$(dir)/Makefile)
