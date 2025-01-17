irqdir = sys/irq
irq_builddir = $(builddir)/$(irqdir)
irq_objs = idt.o irq.o isr.o
irq_asmobjs = idt.asm.o irq.asm.o
irq_objfiles = $(foreach obj, $(irq_objs), $(irqdir)/$(obj))
irq_asmobjfiles = $(foreach obj, $(irq_asmobjs), $(irqdir)/$(obj))

irq: irq_mkdir $(irq_objfiles) $(irq_asmobjfiles)

irq_mkdir:
	mkdir -p $(irq_builddir)

$(filter %.o, $(irq_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

$(filter %.asm.o, $(irq_asmobjfiles)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) -o $(builddir)/$@ $<
