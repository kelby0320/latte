process_subdirs = proc
process_subdirs_targets = $(foreach dir, $(process_subdirs), process_$(dir))
processdir = sys/process
process_builddir = $(builddir)/$(processdir)
process_objs = elf.o fd.o ld.o mmap.o process.o
process_objfiles = $(foreach obj, $(process_objs), $(processdir)/$(obj))

process: process_mkdir $(process_objfiles) $(process_subdirs_targets)

process_mkdir:
	mkdir -p $(process_builddir)

$(filter %.o, $(process_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

include $(foreach dir, $(process_subdirs), sys/process/$(dir)/Makefile)
