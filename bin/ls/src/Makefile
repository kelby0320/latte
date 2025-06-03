lsdir = src
ls_builddir = $(builddir)/bin/ls/$(lsdir)
ls_objs = ls.o
ls_objfiles = $(foreach obj, $(ls_objs), $(lsdir)/$(obj))

ls: ls_mkdir $(ls_objfiles)

ls_mkdir:
	mkdir -p $(ls_builddir)

$(filter %.o, $(ls_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/bin/ls/$@ -c $<
