driver_platform_consoledir = sys/driver/platform/console
driver_platform_console_builddir = $(builddir)/$(driver_platform_consoledir)
driver_platform_console_objs = console.o console_read.o console_write.o
driver_platform_console_objfiles = $(foreach obj, $(driver_platform_console_objs), $(driver_platform_consoledir)/$(obj))

driver_platform_console: driver_platform_console_mkdir $(driver_platform_console_objfiles)

driver_platform_console_mkdir:
	mkdir -p $(driver_platform_console_builddir)

$(filter %.o, $(driver_platform_console_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
