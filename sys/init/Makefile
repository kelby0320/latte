initdir = sys/init
init_builddir = $(builddir)/$(initdir)
init_objs = main.o msgbuf.o term.o
init_objfiles = $(foreach obj, $(init_objs), $(initdir)/$(obj))

init: init_mkdir $(init_objfiles)

init_mkdir:
	mkdir -p $(init_builddir)

$(filter %.o, $(init_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
