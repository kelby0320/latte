%define KERNEL_VIRT_BASE_ADDR 0xC0100000
%define to_paddr(addr) ((addr - KERNEL_VIRT_BASE_ADDR))
%define KERNEL_CODE_SEGMENT     0x08
%define KERNEL_DATA_SEGMENT     0x10
%define USER_CODE_SEGMENT       (0x18 | 0x3)
%define USER_DATA_SEGMENT       (0x20 | 0x3)

global gdt_load

section .boot.text

; void
; gdt_load(struct gdtr *gdtr)
gdt_load:
    push    ebp
    mov     ebp, esp
    mov     ebx, [ebp+8]                ; *gdtr
    lgdt    [ebx]
    pop     ebp
    ret

section .data

gdt_start:
gdt:
	; Null Descriptor
	dd 		0
	dd		0

	; Kernel Code Segment
	dw		0xFFFF						; Limit 15-0
	dw		0x0000						; Base 15-0
	db		0x00						; Base 23-16
	db		0x9A						; Access byte
	db		0xCF						; Flags and Limit 19-16
	db		0x00						; Base 31-24

	; Kernel Data Segment
	dw		0xFFFF						; Limit 15-0
	dw		0x0000						; Base 15-0
	db		0x00						; Base 23-16
	db		0x92						; Access byte
	db		0xCF						; Flags and Limit 19-16
	db		0x00						; Base 31-24

	; User Code Segment
	dw		0xFFFF						; Limit 15-0
	dw		0x0000						; Base 15-0
	db		0x00						; Base 23-16
	db		0xF8						; Access byte
	db		0xCF						; Flags and Limit 19-16
	db		0x00						; Base 31-24

	; User Data Segment
	dw		0xFFFF						; Limit 15-0
	dw		0x0000						; Base 15-0
	db		0x00						; Base 23-16
	db		0xF2						; Access byte
	db		0xCF						; Flags and Limit 19-16
	db		0x00						; Base 31-24

	; TSS Segment
	dd 		0
	dd 		0
gdt_end:

gdtr:
	; gdtr goes here