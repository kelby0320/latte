ENTRY(_start)

SECTIONS
{
	/* Begin loading sections at 1 MiB */
	. = 0x100000;

	_kernel_start = .;

	.boot.text ALIGN(4K)
	{
		*(.multiboot)
		*(.boot.text)
	}

	.boot.bss ALIGN(4K)
	{
		*(.boot.bss)
	}

	/* The kernel will live at 3GB + 1MB */
	. = 0xC0100000;

	.text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}

	.rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	.data ALIGN(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(.COMMON)
		*(.bss)
	}

	_kernel_end = .;
}
