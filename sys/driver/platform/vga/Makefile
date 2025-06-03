driver_platform_vgadir = sys/driver/platform/vga
driver_platform_vga_builddir = $(builddir)/$(driver_platform_vgadir)
driver_platform_vga_objs = vga.o
driver_platform_vga_objfiles = $(foreach obj, $(driver_platform_vga_objs), $(driver_platform_vgadir)/$(obj))

driver_platform_vga: driver_platform_vga_mkdir $(driver_platform_vga_objfiles)

driver_platform_vga_mkdir:
	mkdir -p $(driver_platform_vga_builddir)

$(filter %.o, $(driver_platform_vga_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
