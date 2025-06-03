bootdir = sys/boot
boot_builddir = $(builddir)/$(bootdir)
boot_objs = boot.o multiboot2.o
boot_asmobjs = boot.asm.o header.asm.o
boot_objfiles = $(foreach obj, $(boot_objs), $(bootdir)/$(obj))
boot_asmobjfiles = $(foreach obj, $(boot_asmobjs), $(bootdir)/$(obj))

boot: boot_mkdir $(boot_objfiles) $(boot_asmobjfiles)

boot_mkdir:
	mkdir -p $(boot_builddir)

$(filter %.o, $(boot_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

$(filter %.asm.o, $(boot_asmobjfiles)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) -o $(builddir)/$@ $<
