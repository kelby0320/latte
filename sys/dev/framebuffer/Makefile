dev_framebufferdir = sys/dev/framebuffer
dev_framebuffer_builddir = $(builddir)/$(dev_framebufferdir)
dev_framebuffer_objs = fb_device.o
dev_framebuffer_objfiles = $(foreach obj, $(dev_framebuffer_objs), $(dev_framebufferdir)/$(obj))

dev_framebuffer: dev_framebuffer_mkdir $(dev_framebuffer_objfiles)

dev_framebuffer_mkdir:
	mkdir -p $(dev_framebuffer_builddir)

$(filter %.o, $(dev_framebuffer_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
