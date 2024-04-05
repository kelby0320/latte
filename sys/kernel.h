#ifndef KERNEL_H
#define KERNEL_H

#include "mm/vm.h"

#define KALLOC_PADDR_START      ((void *)0x4400000)  // 68MB
#define KERNEL_HEAP_VADDR_START ((void *)0xC4400000) // 3GB + 68MB
#define USER_HEAP_VADDR_START   ((void *)0x01000000) // 16MB

void
panic(const char *str);

void
switch_to_kernel_vm_area();

#endif
