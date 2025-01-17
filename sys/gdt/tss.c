#include "tss.h"

#include "config.h"
#include "kernel.h"
#include "libk/print.h"
#include "libk/memory.h"
#include "kalloc.h"
#include "paging.h"
#include "vm.h"

struct tss tss;

extern struct vm_area kernel_vm_area;

static void
tss_map_esp()
{
    size_t order = kalloc_size_to_order(TSS_STACK_SIZE);
    void *tss_stack = kalloc_get_phys_pages(order);
    if (!tss_stack) {
        panic("Failed to allocate TSS stack\n");
    }

    // Map the stack from the bottom up.
    // Note: We need to map one extra page to cover the top of stack address
    int res = vm_area_map_pages_to(&kernel_vm_area, (void *)TSS_STACK_BOTTOM, tss_stack,
                                   tss_stack + TSS_STACK_SIZE + PAGING_PAGE_SIZE,
                                   PAGING_PAGE_PRESENT | PAGING_PAGE_WRITABLE);

    if (res < 0) {
        panic("Failed to map TSS stack\n");
    }
}

void
tss_init()
{
    printk("Init TSS at %d\n", (int)&tss);
    
    memset(&tss, 0, sizeof(tss));
    tss.esp0 = TSS_STACK_TOP;
    tss.ss0 = LATTE_KERNEL_DATA_SEGMENT;

    tss_map_esp();

    tss_load(LATTE_TSS_SEGMENT);

    printk("Loaded TSS\n");
}
