syscall_processdir = sys/syscall/process
syscall_process_builddir = $(builddir)/$(syscall_processdir)
syscall_process_objs = exec.o exit.o fork.o wait.o
syscall_process_objfiles = $(foreach obj, $(syscall_process_objs), $(syscall_processdir)/$(obj))

syscall_process: syscall_process_mkdir $(syscall_process_objfiles)

syscall_process_mkdir:
	mkdir -p $(syscall_process_builddir)

$(filter %.o, $(syscall_process_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
