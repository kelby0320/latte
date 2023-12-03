#include "gdt/tss.h"

#include "config.h"
#include "libk/memory.h"

struct tss tss;

void
tss_init()
{
    memset(&tss, 0, sizeof(tss));
    tss.esp0 = LATTE_TSS_ESP;
    tss.ss0 = LATTE_KERNEL_DATA_SEGMENT;
}