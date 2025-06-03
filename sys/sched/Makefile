scheddir = sys/sched
sched_builddir = $(builddir)/$(scheddir)
sched_objs = sched.o
sched_objfiles = $(foreach obj, $(sched_objs), $(scheddir)/$(obj))

sched: sched_mkdir $(sched_objfiles)

sched_mkdir:
	mkdir -p $(sched_builddir)

$(filter %.o, $(sched_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
