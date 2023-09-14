	.section .text
	.global gdt_load
gdt_load:
	movw 4(%esp), %ax		# Size
	movw %ax, gdt_descriptor
	movl 8(%esp), %eax		# Pointer to GDT
	mov %eax, gdt_descriptor+2
	lgdtl gdt_descriptor
	ret

	.section .data
gdt_descriptor:
	.word 0				# Size
	.long 0				# Offset
