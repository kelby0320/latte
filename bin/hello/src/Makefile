hellodir = src
hello_builddir = $(builddir)/bin/hello/$(hellodir)
hello_objs = hello.o
hello_objfiles = $(foreach obj, $(hello_objs), $(hellodir)/$(obj))

hello: hello_mkdir $(hello_objfiles)

hello_mkdir:
	mkdir -p $(hello_builddir)

$(filter %.o, $(hello_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/bin/hello/$@ -c $<
