blockdir = sys/block
block_builddir = $(builddir)/$(blockdir)
block_objs = block.o buffered_reader.o disk.o partition.o
block_objfiles = $(foreach obj, $(block_objs), $(blockdir)/$(obj))

block: block_mkdir $(block_objfiles)

block_mkdir:
	mkdir -p $(block_builddir)

$(filter %.o, $(block_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
