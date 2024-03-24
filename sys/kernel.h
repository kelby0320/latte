#ifndef KERNEL_H
#define KERNEL_H

#include "mm/vm.h"

#define KALLOC_PADDR_START      ((void *)0x2800000)  // 40MB
#define KERNEL_HEAP_VADDR_START ((void *)0xC2800000) // 3GB + 40MB
#define USER_HEAP_VADDR_START   ((void *)0x01000000) // 16MB

struct vm_area kernel_vm_area;

void
panic(const char *str);

void
switch_to_kernel_vm_area();

#endif
