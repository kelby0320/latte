gdtdir = sys/gdt
gdt_builddir = $(builddir)/$(gdtdir)
gdt_objs = gdt.o tss.o
gdt_asmobjs = gdt.asm.o tss.asm.o
gdt_objfiles = $(foreach obj, $(gdt_objs), $(gdtdir)/$(obj))
gdt_asmobjfiles = $(foreach obj, $(gdt_asmobjs), $(gdtdir)/$(obj))

gdt: gdt_mkdir $(gdt_objfiles) $(gdt_asmobjfiles)

gdt_mkdir:
	mkdir -p $(gdt_builddir)

$(filter %.o, $(gdt_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

$(filter %.asm.o, $(gdt_asmobjfiles)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) -o $(builddir)/$@ $<
