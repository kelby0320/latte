driver_platform_atadir = sys/driver/platform/ata
driver_platform_ata_builddir = $(builddir)/$(driver_platform_atadir)
driver_platform_ata_objs = ata.o ata_io.o
driver_platform_ata_objfiles = $(foreach obj, $(driver_platform_ata_objs), $(driver_platform_atadir)/$(obj))

driver_platform_ata: driver_platform_ata_mkdir $(driver_platform_ata_objfiles)

driver_platform_ata_mkdir:
	mkdir -p $(driver_platform_ata_builddir)

$(filter %.o, $(driver_platform_ata_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
