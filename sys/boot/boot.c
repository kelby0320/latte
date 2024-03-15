#include "gdt/gdt.h"
#include "mm/vm.h"

#include <stdint.h>

#define KERNEL_HIGHER_HALF_START 0xC0000000
#define NUM_KERNEL_PAGE_TABLES   8
#define PAGE_DIRECTORY_ENTRIES   1024
#define PAGE_TABLE_ENTRIES       1024
#define PAGE_SIZE                4096
#define PAGE_PRESENT             0b00000001
#define PAGE_WRITABLE            0b00000010
#define to_paddr(addr)           ((void *)((uint32_t)addr - KERNEL_HIGHER_HALF_START))

extern void
boot_enable_paging();

extern void
boot_load_page_directory(uint32_t *page_dir);

uint32_t kernel_page_directory[PAGE_DIRECTORY_ENTRIES];
uint32_t kernel_page_tables[NUM_KERNEL_PAGE_TABLES][PAGE_TABLE_ENTRIES];

/**
 * @brief Initialize a single kernel page table
 * 
 */
__attribute__((section(".boot.text"))) static void
init_page_table(uint32_t *page_table, uint32_t tbl_offset)
{
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        uint32_t tbl_entry = (tbl_offset + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;

        /* kernel_page_table_0 is located in high memory (e.g above 0xC0100000).
         * The physical address it is loaded at however is above 0x00100000.
         * To correctly access the memory we need to convert the high mem address to
         * the real physical address.
         */
        uint32_t *phys_kernel_page_table = to_paddr(page_table);

        phys_kernel_page_table[i] = tbl_entry;
    }
}

/**
 * @brief Initialize kernel page tables
 * 
 */
__attribute__((section(".boot.text"))) static void
init_kernel_page_tables()
{
    for (int i = 0; i < NUM_KERNEL_PAGE_TABLES; i++) {
        uint32_t tbl_offset = i * PAGE_TABLE_ENTRIES * PAGE_SIZE;
        init_page_table(kernel_page_tables[i], tbl_offset);
    }
}

/**
 * @brief Map kernel page tables
 * 
 */
__attribute__((section(".boot.text"))) static void
map_kernel_page_tables()
{
    for (int i = 0; i < NUM_KERNEL_PAGE_TABLES; i++) {
        /* kernel_page_tables is located in high memory (e.g above 0xC0100000).
        * The physical address it is loaded at however is above 0x00100000.
        * To correctly access the memory we need to convert the high mem address to
        * the real physical address.
        */
        uint32_t *phys_kernel_page_table = to_paddr(kernel_page_tables[i]);
        uint32_t dir_entry = (uint32_t)phys_kernel_page_table | PAGE_PRESENT | PAGE_WRITABLE;

        /* The same conversion must be done to kernel_page_directory */
        uint32_t *phys_kernel_page_directory = to_paddr(kernel_page_directory);

        phys_kernel_page_directory[i] = dir_entry;  // Identity map low mem
        phys_kernel_page_directory[i+768] = dir_entry; // Map addresses above 0xC0000000 to low mem
    }
}

/**
 * @brief Setup initial higher half kernel page mapping
 *
 */
__attribute__((section(".boot.text"))) static void
map_kernel_pages()
{
    init_kernel_page_tables();
    map_kernel_page_tables();
}

/**
 * @brief Remove the mapping for low memory
 *
 */
static void
unmap_low_mem()
{
    for (int i = 0; i < NUM_KERNEL_PAGE_TABLES; i++) {
        kernel_page_directory[i] = 0;
    }
}

/**
 * @brief Early boot initialization
 *
 * Setup higher half page table and enable paging
 * Note: Code in this function can only call functions with
 * __attribute__(section(".boot.text")))
 *
 */
__attribute__((section(".boot.text"))) void
early_init()
{
    map_kernel_pages();
    boot_load_page_directory(to_paddr(kernel_page_directory));
    boot_enable_paging();
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
    gdt_set_kernel_data_segment();
}