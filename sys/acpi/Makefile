acpidir = sys/acpi
acpi_builddir = $(builddir)/$(acpidir)
acpi_objs = acpi.o
acpi_objfiles = $(foreach obj, $(acpi_objs), $(acpidir)/$(obj))

acpi: acpi_mkdir $(acpi_objfiles)

acpi_mkdir:
	mkdir -p $(acpi_builddir)

$(filter %.o, $(acpi_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
