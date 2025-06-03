driver_platform_kbddir = sys/driver/platform/kbd
driver_platform_kbd_builddir = $(builddir)/$(driver_platform_kbddir)
driver_platform_kbd_objs = kbd.o ps2.o
driver_platform_kbd_objfiles = $(foreach obj, $(driver_platform_kbd_objs), $(driver_platform_kbddir)/$(obj))

driver_platform_kbd: driver_platform_kbd_mkdir $(driver_platform_kbd_objfiles)

driver_platform_kbd_mkdir:
	mkdir -p $(driver_platform_kbd_builddir)

$(filter %.o, $(driver_platform_kbd_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
