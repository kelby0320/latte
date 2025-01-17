mm_subdirs = paging
mm_subdirs_targets = $(foreach dir, $(mm_subdirs), mm_$(dir))
mmdir = sys/mm
mm_builddir = $(builddir)/$(mmdir)
mm_objs = buddy.o kalloc.o vm.o
mm_objfiles = $(foreach obj, $(mm_objs), $(mmdir)/$(obj))

mm: mm_mkdir $(mm_objfiles) $(mm_subdirs_targets)

mm_mkdir:
	mkdir -p $(mm_builddir)

$(filter %.o, $(mm_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

include $(foreach dir, $(mm_subdirs), sys/mm/$(dir)/Makefile)
