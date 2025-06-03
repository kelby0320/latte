driver_subdirs = framebuffer platform
driver_subdirs_targets = $(foreach dir, $(driver_subdirs), driver_$(dir))
driverdir = sys/driver
driver_builddir = $(builddir)/$(driverdir)
driver_objs = driver.o
driver_objfiles = $(foreach obj, $(driver_objs), $(driverdir)/$(obj))

driver: driver_mkdir $(driver_objfiles) $(driver_subdirs_targets)

driver_mkdir:
	mkdir -p $(driver_builddir)

$(filter %.o, $(driver_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

include $(foreach dir, $(driver_subdirs), sys/driver/$(dir)/Makefile)
