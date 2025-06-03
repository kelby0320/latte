mm_pagingdir = sys/mm/paging
mm_paging_builddir = $(builddir)/$(mm_pagingdir)
mm_paging_objs = page_dir.o page_tbl.o paging.o
mm_paging_asmobjs = paging.asm.o
mm_paging_objfiles = $(foreach obj, $(mm_paging_objs), $(mm_pagingdir)/$(obj))
mm_paging_asmobjfiles = $(foreach obj, $(mm_paging_asmobjs), $(mm_pagingdir)/$(obj))

mm_paging: mm_paging_mkdir $(mm_paging_objfiles) $(mm_paging_asmobjfiles)

mm_paging_mkdir:
	mkdir -p $(mm_paging_builddir)

$(filter %.o, $(mm_paging_objfiles)): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(builddir)/$@ -c $<

$(filter %.asm.o, $(mm_paging_asmobjfiles)): %.asm.o: %.asm
	$(AS) $(ASFLAGS) -o $(builddir)/$@ $<
