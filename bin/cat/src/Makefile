catdir = src
cat_builddir = $(builddir)/bin/cat/$(catdir)
cat_objs = cat.o
cat_objfiles = $(foreach obj, $(cat_objs), $(catdir)/$(obj))

cat: cat_mkdir $(cat_objfiles)

cat_mkdir:
	mkdir -p $(cat_builddir)

$(filter %.o, $(cat_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/bin/cat/$@ -c $<
