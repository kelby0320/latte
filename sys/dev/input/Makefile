dev_inputdir = sys/dev/input
dev_input_builddir = $(builddir)/$(dev_inputdir)
dev_input_objs = input_device.o keycode.o
dev_input_objfiles = $(foreach obj, $(dev_input_objs), $(dev_inputdir)/$(obj))

dev_input: dev_input_mkdir $(dev_input_objfiles)

dev_input_mkdir:
	mkdir -p $(dev_input_builddir)

$(filter %.o, $(dev_input_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<
