threaddir = sys/thread
thread_builddir = $(builddir)/$(threaddir)
thread_objs = thread.o userio.o
thread_asmobjs = thread.asm.o
thread_objfiles = $(foreach obj, $(thread_objs), $(threaddir)/$(obj))
thread_asmobjfiles = $(foreach obj, $(thread_asmobjs), $(threaddir)/$(obj))

thread: thread_mkdir $(thread_objfiles) $(thread_asmobjfiles)

thread_mkdir:
	mkdir -p $(thread_builddir)

$(filter %.o, $(thread_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

$(filter %.asm.o, $(thread_asmobjfiles)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) -o $(builddir)/$@ $<
