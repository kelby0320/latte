	# Constants
	.set ALIGN, 	1<<0			# Align loaded modules on page boundaries
	.set MEMINFO,	1<<1			# Provide memory map
	.set FLAGS,	ALIGN | MEMINFO		# Multiboot 'flag' field
	.set MAGIC, 	0x1BADB002		# 'Magic Number'
	.set CHECKSUM, 	-(MAGIC + FLAGS)	# Checksum to prove we are multiboot

	# Multiboot header
	.section .multiboot
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

	# Reserver kernel stack
	.section .bss
stack_bottom:
	.skip 16384				# 16 Kib
stack_top:

	# Kernel entry point
	.section .text
	.global _start
	.type _start, @function
_start:
	movl $stack_top, %esp
	call kernel_main

	# Hang if kernel returns
	cli
1:	hlt
	jmp 1b
	
