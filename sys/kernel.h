#ifndef KERNEL_H
#define KERNEL_H

void
print(const char *str);

void
panic(const char *str);

void
switch_to_kernel_vm_area();


#endif
