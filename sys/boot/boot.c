#include "gdt/gdt.h"
#include "mm/vm.h"

#include "cpu/port.h"

#include <stdint.h>

#define KERNEL_HIGHER_HALF_START 0xC0000000
#define PAGE_DIRECTORY_ENTRIES   1024
#define PAGE_TABLE_ENTRIES       1024
#define PAGE_SIZE                4096
#define NUM_KERNEL_PAGE_TABLES   16
#define KERNEL_DIR_ENTRIES       256
#define KERNEL_DIR_OFFSET        768
#define PAGE_PRESENT             0b00000001
#define PAGE_WRITABLE            0b00000010
#define to_paddr(addr)           ((void *)((uint32_t)addr - KERNEL_HIGHER_HALF_START))

extern void
boot_enable_paging();

extern void
boot_load_page_directory(uint32_t *page_dir);

__attribute__((aligned(4096)))
uint32_t kernel_page_directory[PAGE_DIRECTORY_ENTRIES] = {0};

__attribute__((aligned(4096)))
uint32_t kernel_page_tables[PAGE_DIRECTORY_ENTRIES][PAGE_TABLE_ENTRIES] = {0};

/**
 * @brief Initialize a single kernel page table
 *
 */
__attribute__((section(".boot.text"))) static void
init_page_table(uint32_t *page_table, uint32_t page_tbl_offset, uint8_t flags)
{
    /* kernel_page_table_0 is located in high memory (e.g above 0xC0100000).
     * The physical address it is loaded at however is above 0x00100000.
     * To correctly access the memory we need to convert the high mem address to
     * the real physical address.
     */
    uint32_t *phys_kernel_page_table = to_paddr(page_table);

    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        uint32_t tbl_entry = (page_tbl_offset + (i * PAGE_SIZE));
        tbl_entry &= 0xFFFFF000; // Clear flags
        tbl_entry |= flags;
        phys_kernel_page_table[i] = tbl_entry;
    }
}

/**
 * @brief Intialize kernel page tables
 *
 * Identity map the first 3GB of memory
 *
 */
__attribute__((section(".boot.text"))) static void
init_kernel_page_tables()
{
    for (int i = 0; i < PAGE_DIRECTORY_ENTRIES - KERNEL_DIR_ENTRIES; i++) {
        uint32_t tbl_offset = i * PAGE_TABLE_ENTRIES * PAGE_SIZE;
        uint8_t flags = PAGE_PRESENT | PAGE_WRITABLE;

        init_page_table(kernel_page_tables[i], tbl_offset, flags);
    }
}

/**
 * @brief Map kernel page directories
 *
 */
__attribute__((section(".boot.text"))) static void
map_kernel_directory()
{

    /* kernel_page_directory is located in high memory (e.g above 0xC0100000).
     * The physical address it is loaded at however is above 0x00100000.
     * To correctly access the memory we need to convert the high mem address to
     * the real physical address.
     */
    uint32_t *phys_kernel_page_directory = to_paddr(kernel_page_directory);

    // Identity map all memory
    for (int i = 0; i < PAGE_DIRECTORY_ENTRIES - KERNEL_DIR_ENTRIES; i++) {
        /* The same conversion must be done to kernel_page_tables[i] */
        uint32_t *phys_kernel_page_table = to_paddr(kernel_page_tables[i]);
        uint32_t dir_entry = (uint32_t)phys_kernel_page_table | PAGE_PRESENT | PAGE_WRITABLE;

        phys_kernel_page_directory[i] = dir_entry;
    }

    // Map the bottom 1GB to the top 1GB
    for (int i = 0; i < NUM_KERNEL_PAGE_TABLES; i++) {
        uint32_t *phys_kernel_page_table = to_paddr(kernel_page_tables[i]);
        uint32_t dir_entry = (uint32_t)phys_kernel_page_table | PAGE_PRESENT | PAGE_WRITABLE;

        phys_kernel_page_directory[i + PAGING_KERNEL_DIR_OFFSET] = dir_entry;
    }
}

/**
 * @brief Setup initial higher half kernel page mapping
 *
 */
__attribute__((section(".boot.text"))) static void
setup_kernel_memory_map()
{
    init_kernel_page_tables();
    map_kernel_directory();
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
    setup_kernel_memory_map();
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
    paging_flush_tlb();
    gdt_init();
    gdt_set_kernel_data_segment();
}
