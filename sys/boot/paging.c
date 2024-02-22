#define KERNEL_VIRT_BASE_ADDR 0xC0100000
#define to_paddr(addr)        ((addr - KERNEL_VIRT_BASE_ADDR))

__attribute__((section(".boot.text"))) void
map_kernel_page_tables()
{
    // TODO
}

__attribute__((section(".boot.text"))) void
unmap_low_mem()
{
    // TODO
}