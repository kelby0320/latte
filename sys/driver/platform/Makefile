driver_platform_subdirs = ata console devfs kbd vga
driver_platform_subdirs_targets = $(foreach dir, $(driver_platform_subdirs), driver_platform_$(dir))
driver_platformdir = sys/driver/platform
driver_platform_builddir = $(builddir)/$(driver_platformdir)
driver_platform_objs = platform_driver.o
driver_platform_objfiles = $(foreach obj, $(driver_platform_objs), $(driver_platformdir)/$(obj))

driver_platform: driver_platform_mkdir $(driver_platform_objfiles) $(driver_platform_subdirs_targets)

driver_platform_mkdir:
	mkdir -p $(driver_platform_builddir)

$(filter %.o, $(driver_platform_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

include $(foreach dir, $(driver_platform_subdirs), sys/driver/platform/$(dir)/Makefile)
