#include "kernel.h"

#include "boot/multiboot2.h"
#include "bus/bus.h"
#include "config.h"
#include "fs/fs.h"
#include "gdt/gdt.h"
#include "gdt/tss.h"
#include "irq/irq.h"
#include "libk/kheap.h"
#include "libk/print.h"
#include "libk/string.h"
#include "mm/vm.h"
#include "msgbuf.h"
#include "task/process.h"
#include "task/sched.h"
#include "vfs/vfs.h"

extern uint32_t kernel_page_directory;

static struct vm_area kernel_vm_area;

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
    vm_area_switch_map(&kernel_vm_area);
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
    vm_area_from_page_directory(&kernel_vm_area, &kernel_page_directory);

    kheap_init();

    // int res = multiboot2_verify_magic_number(magic);
    // if (!res) {
    //     panic("Invalid multiboot magic number\n");
    // }

    irq_init();

    tss_init();

    tss_load(LATTE_TSS_SEGMENT);
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
    bus_init();

    bus_probe();

    int res = fs_init();
    if (res < 0) {
        panic("Failed to initialize filesystem drivers\n");
    }

    res = vfs_init();
    if (res < 0) {
        panic("Failed to initialize virtual filesystem\n");
    }
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

    int out_fd = vfs_open("/dev/term0", "w");
    msgbuf_add_output_fd(out_fd);

    char buf[1024];
    int fd = vfs_open("/latte.txt", "r");
    vfs_read(fd, buf, 32);
    printk(buf);

    process_spawn("/bin/hello");
    schedule_first_task();

    while (1) {}
}
