dev_platformdir = sys/dev/platform
dev_platform_builddir = $(builddir)/$(dev_platformdir)
dev_platform_objs = platform_device.o
dev_platform_objfiles = $(foreach obj, $(dev_platform_objs), $(dev_platformdir)/$(obj))

dev_platform: dev_platform_mkdir $(dev_platform_objfiles)

dev_platform_mkdir:
	mkdir -p $(dev_platform_builddir)

$(filter %.o, $(dev_platform_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
