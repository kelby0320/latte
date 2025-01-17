driver_framebufferdir = sys/driver/framebuffer
driver_framebuffer_builddir = $(builddir)/$(driver_framebufferdir)
driver_framebuffer_objs = fb_driver.o
driver_framebuffer_objfiles = $(foreach obj, $(driver_framebuffer_objs), $(driver_framebufferdir)/$(obj))

driver_framebuffer: driver_framebuffer_mkdir $(driver_framebuffer_objfiles)

driver_framebuffer_mkdir:
	mkdir -p $(driver_framebuffer_builddir)

$(filter %.o, $(driver_framebuffer_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
