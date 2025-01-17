#include "kernel.h"

#include "multiboot2.h"
#include "config.h"
#include "cpu.h"
#include "bus.h"
#include "driver.h"
#include "fs.h"
#include "gdt.h"
#include "tss.h"
#include "irq.h"
#include "msgbuf.h"
#include "term.h"
#include "libk/alloc.h"
#include "libk/print.h"
#include "libk/string.h"
#include "kalloc.h"
#include "vm.h"
#include "process.h"
#include "sched.h"
#include "vfs.h"

extern uint32_t kernel_page_directory;

struct vm_area kernel_vm_area;

/**
 * @brief Output a string and halt
 *
 * @param str Message to output
 */
void
panic(const char *str)
{
    printk("Kernel Panic! %s", str);
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
    vm_area_kernel_init(&kernel_vm_area, (page_dir_t)&kernel_page_directory);

    // 512 MB heap size for now
    // Should be passed to kernel_main by boot code
    kalloc_init(KALLOC_PADDR_START, (1024 * 1024 * 512));

    libk_alloc_init();

    // int res = multiboot2_verify_magic_number(magic);
    // if (!res) {
    //     panic("Invalid multiboot magic number\n");
    // }

    irq_init();

    tss_init();

    cpu_init();
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

    driver_init();

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

    int out_fd = vfs_open("/dev/console", "w");
    if (out_fd < 0) {
	panic("Failed to open /dev/console\n");
    }

    term_clear_screen();
    msgbuf_add_output_fd(out_fd);

    enable_interrupts();

    printk("Starting init program\n\n");
    
    process_create_first("/bin/init");
    schedule_first_thread();

    while (1) {}
}
