#include "gdt/gdt.h"
#include "mem/vm.h"

#include <stdint.h>

#define KERNEL_HIGHER_HALF_START 0xC0000000
#define PAGE_DIRECTORY_ENTRIES   1024
#define PAGE_TABLE_ENTRIES       1024
#define PAGE_SIZE                1024
#define PAGE_PRESENT             0b00000001
#define PAGE_WRITABLE            0b00000010
#define to_paddr(addr)           ((addr - KERNEL_HIGHER_HALF_START))

uint32_t kernel_page_directory[PAGE_DIRECTORY_ENTRIES];
uint32_t kernel_page_table_0[PAGE_TABLE_ENTRIES];

/**
 * @brief Identity map page table 0
 *
 */
__attribute__((section(".boot.text"))) static void
init_page_table_0()
{
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        uint32_t tbl_entry = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;

        /* kernel_page_table_0 is located in high memory (e.g above 0xC0100000).
         * The physical address it is loaded at however is above 0x00100000.
         * To correctly access the memory we need to convert the high mem address to
         * the real physical address.
         */
        uint32_t *phys_kernel_page_table_0 = to_paddr(kernel_page_table_0);

        phys_kernel_page_table_0[i] = tbl_entry;
    }
}

/**
 * @brief Setup initial higher half kernel page mapping
 *
 */
__attribute__((section(".boot.text"))) static void
map_kernel_pages()
{
    init_page_table_0();

    /* kernel_page_table_0 is located in high memory (e.g above 0xC0100000).
     * The physical address it is loaded at however is above 0x00100000.
     * To correctly access the memory we need to convert the high mem address to
     * the real physical address.
     */
    uint32_t *phys_kernel_page_table_0 = to_paddr(kernel_page_table_0);
    uint32_t dir_entry = (uint32_t)phys_kernel_page_table_0 | PAGE_PRESENT | PAGE_WRITABLE;

    /* The same conversion must be done to kernel_page_directory */
    uint32_t *phys_kernel_page_directory = to_paddr(kernel_page_directory);
    phys_kernel_page_directory[0] = dir_entry;
    phys_kernel_page_directory[3072] = dir_entry; // Map 0xC0000000 to the page table 0
}

/**
 * @brief Remove the mapping for page directory entry 0
 *
 */
static void
unmap_low_mem()
{
    kernel_page_directory[0] = 0;
}

/**
 * @brief Early boot initialization
 *
 * Setup higher half page table and enable paging
 *
 */
__attribute__((section(".boot.text"))) void
early_init()
{
    map_kernel_pages();
    load_page_directory(to_paddr(kernel_page_directory));
    enable_paging();
}

/**
 * @brief Late boot initialization
 *
 * Unmap low memory and setup a GDT
 */
void
late_init()
{
    unmap_low_mem();
    flush_tlb();
    gdt_init();
}