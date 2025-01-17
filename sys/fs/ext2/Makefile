fs_ext2dir = sys/fs/ext2
fs_ext2_builddir = $(builddir)/$(fs_ext2dir)
fs_ext2_objs = block_iter.o dir.o dir_iter.o ext2.o inode.o
fs_ext2_objfiles = $(foreach obj, $(fs_ext2_objs), $(fs_ext2dir)/$(obj))

fs_ext2: fs_ext2_mkdir $(fs_ext2_objfiles)

fs_ext2_mkdir:
	mkdir -p $(fs_ext2_builddir)

$(filter %.o, $(fs_ext2_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

