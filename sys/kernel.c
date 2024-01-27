#include "kernel.h"

#include "boot/multiboot2.h"
#include "bus/bus.h"
#include "config.h"
#include "dev/device.h"
#include "fs/fs.h"
#include "gdt/gdt.h"
#include "gdt/tss.h"
#include "irq/irq.h"
#include "libk/kheap.h"
#include "libk/print.h"
#include "libk/string.h"
#include "mem/vm.h"
#include "task/process.h"
#include "task/sched.h"
#include "vfs/vfs.h"

static struct vm_area kernel_area;

/**
 * @brief Output a string and halt
 *
 * @param str Message to output
 */
void
panic(const char *str)
{
    printk(str);
    while (1) {}
}

/**
 * @brief Switch to the kernel page table
 *
 */
void
switch_to_kernel_vm_area()
{
    gdt_set_kernel_data_segment();
    vm_area_switch_map(&kernel_area);
}

/**
 * @brief Early kernel intialization code
 *
 * This code runs BEFORE the kernel heap and paging have been setup
 *
 * @param magic Multiboot magic number
 */
void
kernel_early_init(unsigned long magic)
{
    int res = multiboot2_verify_magic_number(magic);
    if (!res) {
        panic("Invalid multiboot magic number");
    }

    gdt_init();

    tss_init();

    tss_load(LATTE_TSS_SEGMENT);

    irq_init();
}

/**
 * @brief Late kernel initialization code
 *
 * This code runs AFTER the kernel heap and paging have been setup
 *
 */
void
kernel_late_init()
{
    kheap_init();

    int res = vm_area_init(&kernel_area, VM_PAGE_PRESENT | VM_PAGE_WRITABLE);
    if (res < 0) {
        panic("Failed to initialize kernel vm area");
    }

    switch_to_kernel_vm_area();

    enable_paging();

    bus_init();

    device_init();

    bus_probe();

    fs_init();

    vfs_init();
}

/**
 * @brief Kernel entry point
 *
 * @param magic Multiboot 2 magic number
 */
void
kernel_main(unsigned long magic)
{
    kernel_early_init(magic);

    kernel_late_init();

    char buf[1024];
    int fd = vfs_open("/latte.txt", "r");
    vfs_read(fd, buf, 32);
    printk(buf);

    process_spawn("/bin/testprog");
    schedule_first_task();

    while (1) {}
}
