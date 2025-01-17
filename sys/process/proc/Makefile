process_procdir = sys/process/proc
process_proc_builddir = $(builddir)/$(process_procdir)
process_proc_objs = exec.o exit.o fork.o wait.o
process_proc_objfiles = $(foreach obj, $(process_proc_objs), $(process_procdir)/$(obj))

process_proc: process_proc_mkdir $(process_proc_objfiles)

process_proc_mkdir:
	mkdir -p $(process_proc_builddir)

$(filter %.o, $(process_proc_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
