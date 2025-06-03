dev_subdirs = framebuffer input platform
dev_subdirs_targets = $(foreach dir, $(dev_subdirs), dev_$(dir))
devdir = sys/dev
dev_builddir = $(builddir)/$(devdir)
dev_objs = bus.o device.o
dev_objfiles = $(foreach obj, $(dev_objs), $(devdir)/$(obj))

dev: dev_mkdir $(dev_objfiles) $(dev_subdirs_targets)

dev_mkdir:
	mkdir -p $(dev_builddir)

$(filter %.o, $(dev_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

include $(foreach dir, $(dev_subdirs), sys/dev/$(dir)/Makefile)
