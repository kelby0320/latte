#ifndef KERNEL_H
#define KERNEL_H

#include "mm/vm.h"

#define KALLOC_PADDR_START      ((void *)0x4400000)  // 68MB

void
panic(const char *str);

void
switch_to_kernel_vm_area();

#endif
