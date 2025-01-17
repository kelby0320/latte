cpudir = sys/cpu
cpu_builddir = $(builddir)/$(cpudir)
cpu_objs = cpu.o
cpu_asmobjs = port.asm.o
cpu_objfiles = $(foreach obj, $(cpu_objs), $(cpudir)/$(obj))
cpu_asmobjfiles = $(foreach obj, $(cpu_asmobjs), $(cpudir)/$(obj))

cpu: cpu_mkdir $(cpu_objfiles) $(cpu_asmobjfiles)

cpu_mkdir:
	mkdir -p $(cpu_builddir)

$(filter %.o, $(cpu_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

$(filter %.o, $(cpu_asmobjfiles)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) -o $(builddir)/$@ $<
