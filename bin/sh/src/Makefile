shdir = src
sh_builddir = $(builddir)/bin/sh/$(shdir)
sh_objs = sh.o
sh_objfiles = $(foreach obj, $(sh_objs), $(shdir)/$(obj))

sh: sh_mkdir $(sh_objfiles)

sh_mkdir:
	mkdir -p $(sh_builddir)

$(filter %.o, $(sh_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/bin/sh/$@ -c $<
