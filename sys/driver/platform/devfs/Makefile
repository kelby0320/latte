driver_platform_devfsdir = sys/driver/platform/devfs
driver_platform_devfs_builddir = $(builddir)/$(driver_platform_devfsdir)
driver_platform_devfs_objs = devfs.o
driver_platform_devfs_objfiles = $(foreach obj, $(driver_platform_devfs_objs), $(driver_platform_devfsdir)/$(obj))

driver_platform_devfs: driver_platform_devfs_mkdir $(driver_platform_devfs_objfiles)

driver_platform_devfs_mkdir:
	mkdir -p $(driver_platform_devfs_builddir)

$(filter %.o, $(driver_platform_devfs_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
