#include "kernel.h"

#include "boot/multiboot2.h"
#include "bus/bus.h"
#include "config.h"
#include "dev/device.h"
#include "dev/term/term.h"
#include "fs/fs.h"
#include "gdt/gdt.h"
#include "gdt/tss.h"
#include "irq/irq.h"
#include "libk/libk.h"
#include "libk/print.h"
#include "libk/string.h"
#include "mem/vm.h"
#include "task/process.h"
#include "task/sched.h"
#include "vfs/vfs.h"

static struct vm_area kernel_area;

void
panic(const char *str)
{
    printk(str);
    while (1) {}
}

void
switch_to_kernel_vm_area()
{
    gdt_set_kernel_data_segment();
    vm_area_switch_map(&kernel_area);
}

void
kernel_main(unsigned long magic, void *addr)
{
    terminal_initialize();

    int res = multiboot2_verify_magic_number(magic);
    if (!res) {
        panic("Invalid multiboot magic number");
    }

    // Initialize GDT
    gdt_init();

    // Initialize Task Switch Segment
    tss_init();

    // Load the TSS
    tss_load(LATTE_TSS_SEGMENT);

    // Initialize libk
    libk_init();

    // Initialize IRQs
    irq_init();

    // Initialize kernel vm area
    res = vm_area_init(&kernel_area, VM_PAGE_PRESENT | VM_PAGE_WRITABLE);
    if (res < 0) {
        panic("Failed to initialize kernel vm area");
    }

    // Switch to kernel virtual memory map
    switch_to_kernel_vm_area();

    // Enable virtual memory
    enable_paging();

    // Initialize the bus subsystem
    bus_init();

    // Initialize the device subsystem
    device_init();

    // Probe for devices
    bus_probe();

    // Initialize filesystem drivers
    fs_init();

    // Initialize the virtual filesystem
    vfs_init();

    char buf[1024];
    int fd = vfs_open("/latte.txt", "r");
    vfs_read(fd, buf, 32);
    printk(buf);

    process_spawn("/bin/testprog");
    schedule_first_task();

    while (1) {}
}
