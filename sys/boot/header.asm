
%define MULTIBOOT2_HEADER_MAGIC 0xE85250D6
%define MULTIBOOT2_ARCH_I386 0

; Multiboot header
section .multiboot
align 8

multiboot_header_start:
dd MULTIBOOT2_HEADER_MAGIC
dd MULTIBOOT2_ARCH_I386
dd ((multiboot_header_start + 16) - multiboot_header_start)
dd -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCH_I386 + ((multiboot_header_start + 16) - multiboot_header_start))