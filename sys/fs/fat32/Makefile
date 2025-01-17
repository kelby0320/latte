fs_fat32dir = sys/fs/fat32
fs_fat32_builddir = $(builddir)/$(fs_fat32dir)
fs_fat32_objs = fat32.o
fs_fat32_objfiles = $(foreach obj, $(fs_fat32_objs), $(fs_fat32dir)/$(obj))

fs_fat32: fs_fat32_mkdir $(fs_fat32_objfiles)

fs_fat32_mkdir:
	mkdir -p $(fs_fat32_builddir)

$(filter %.o, $(fs_fat32_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

