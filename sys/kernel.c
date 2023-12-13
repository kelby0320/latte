#include "kernel.h"

#include "dev/disk/disk.h"
#include "dev/term/term.h"
#include "fs/fs.h"
#include "gdt/gdt.h"
#include "irq/irq.h"
#include "libk/libk.h"
#include "mem/vm.h"
#include "task/process.h"
#include "task/sched.h"

static struct vm_area kernel_area;

void
print(const char *str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        terminal_writechar(str[i]);
    }
}

void
panic(const char *str)
{
    print(str);
    while (1) {}
}

void
switch_to_kernel_vm_area()
{
    gdt_set_kernel_data_segment();
    vm_area_switch_map(&kernel_area);
}

void
kernel_main()
{
    terminal_initialize();

    // Initialize GDT
    gdt_init();

    // Initialize libk
    libk_init();

    // Initialize filesystem drivers
    fs_init();

    // Find and Initialize Disks
    disk_probe_and_init();

    // Initialize IRQs
    irq_init();

    // Initialize kernel vm area
    int res = vm_area_init(&kernel_area, VM_PAGE_PRESENT | VM_PAGE_WRITABLE | VM_PAGE_SUPERVISOR);
    if (res < 0) {
        panic("Failed to initialize kernel vm area");
    }

    // Switch to kernel virtual memory map
    vm_area_switch_map(&kernel_area);

    // Enable virtual memory
    enable_paging();

    char buf[1024];
    int  fd = fopen("hdd0:/latte.txt", "r");
    fread(fd, buf, 32);
    print(buf);

    /* int pid = process_new(); */
    /* process_load_exec(pid, "hdd0:/testprog.elf"); */
    /* schedule_first_task();  */

    while (1) {}
}
