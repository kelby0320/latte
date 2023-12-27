/**
 * @file multiboot.h
 * @author Kelby Madal-Hellmuth (kelby0320@gmail.com)
 * @brief Multiboot header structures
 * @version 0.1
 * @date 2023-12-27
 *
 * Much of this code was originally obtained from the sample multiboot2.h file
 * located at https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html
 *
 * @copyright Copyright (c) 2023
 *
 * @license SPDX-License-Identifier: MIT
 */
#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

/*  How many bytes from the start of the file we search for the header. */
#define MULTIBOOT_SEARCH       32768
#define MULTIBOOT_HEADER_ALIGN 8

/*  The magic field should contain this. */
#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6

/*  This should be in %eax. */
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

/*  Alignment of multiboot modules. */
#define MULTIBOOT_MOD_ALIGN 0x00001000

/*  Alignment of the multiboot info structure. */
#define MULTIBOOT_INFO_ALIGN 0x00000008

/*  Flags set in the ’flags’ member of the multiboot header. */

#define MULTIBOOT_TAG_ALIGN                 8
#define MULTIBOOT_TAG_TYPE_END              0
#define MULTIBOOT_TAG_TYPE_CMDLINE          1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME 2
#define MULTIBOOT_TAG_TYPE_MODULE           3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO    4
#define MULTIBOOT_TAG_TYPE_BOOTDEV          5
#define MULTIBOOT_TAG_TYPE_MMAP             6
#define MULTIBOOT_TAG_TYPE_VBE              7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER      8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS     9
#define MULTIBOOT_TAG_TYPE_APM              10
#define MULTIBOOT_TAG_TYPE_EFI32            11
#define MULTIBOOT_TAG_TYPE_EFI64            12
#define MULTIBOOT_TAG_TYPE_SMBIOS           13
#define MULTIBOOT_TAG_TYPE_ACPI_OLD         14
#define MULTIBOOT_TAG_TYPE_ACPI_NEW         15
#define MULTIBOOT_TAG_TYPE_NETWORK          16
#define MULTIBOOT_TAG_TYPE_EFI_MMAP         17
#define MULTIBOOT_TAG_TYPE_EFI_BS           18
#define MULTIBOOT_TAG_TYPE_EFI32_IH         19
#define MULTIBOOT_TAG_TYPE_EFI64_IH         20
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR   21

#define MULTIBOOT_HEADER_TAG_END                 0
#define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST 1
#define MULTIBOOT_HEADER_TAG_ADDRESS             2
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS       3
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS       4
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER         5
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN        6
#define MULTIBOOT_HEADER_TAG_EFI_BS              7
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI32 8
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64 9
#define MULTIBOOT_HEADER_TAG_RELOCATABLE         10

#define MULTIBOOT_ARCHITECTURE_I386   0
#define MULTIBOOT_ARCHITECTURE_MIPS32 4
#define MULTIBOOT_HEADER_TAG_OPTIONAL 1

#define MULTIBOOT_LOAD_PREFERENCE_NONE 0
#define MULTIBOOT_LOAD_PREFERENCE_LOW  1
#define MULTIBOOT_LOAD_PREFERENCE_HIGH 2

#define MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED   1
#define MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED 2

#define MULTIBOOT_MEMORY_AVAILABLE        1
#define MULTIBOOT_MEMORY_RESERVED         2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS              4
#define MULTIBOOT_MEMORY_BADRAM           5

#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED  0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB      1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2

struct header {
    /*  Must be MULTIBOOT_MAGIC - see above. */
    uint32_t magic;

    /*  ISA */
    uint32_t architecture;

    /*  Total header length. */
    uint32_t header_length;

    /*  The above fields plus this one must equal 0 mod 2^32. */
    uint32_t checksum;
} __attribute__((packed));

struct header_tag {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
} __attribute__((packed));

struct header_tag_information_request {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t requests[0];
} __attribute__((packed));

struct header_tag_address {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
} __attribute__((packed));

struct header_tag_entry_address {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t entry_addr;
} __attribute__((packed));

struct header_tag_console_flags {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t console_flags;
} __attribute__((packed));

struct header_tag_framebuffer {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} __attribute__((packed));

struct header_tag_module_align {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
} __attribute__((packed));

struct header_tag_relocatable {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t min_addr;
    uint32_t max_addr;
    uint32_t align;
    uint32_t preference;
} __attribute__((packed));

struct color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} __attribute__((packed));

struct mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
} __attribute__((packed));
typedef struct mmap_entry memory_map_t;

struct tag {
    uint32_t type;
    uint32_t size;
} __attribute__((packed));

struct tag_string {
    uint32_t type;
    uint32_t size;
    char string[0];
} __attribute__((packed));

struct tag_module {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    char cmdline[0];
} __attribute__((packed));

struct tag_basic_meminfo {
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;
    uint32_t mem_upper;
} __attribute__((packed));

struct tag_bootdev {
    uint32_t type;
    uint32_t size;
    uint32_t biosdev;
    uint32_t slice;
    uint32_t part;
} __attribute__((packed));

struct tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct mmap_entry entries[0];
} __attribute__((packed));

struct vbe_info_block {
    uint8_t external_specification[512];
} __attribute__((packed));

struct vbe_mode_info_block {
    uint8_t external_specification[256];
} __attribute__((packed));

struct tag_vbe {
    uint32_t type;
    uint32_t size;

    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    struct vbe_info_block vbe_control_info;
    struct vbe_mode_info_block vbe_mode_info;
} __attribute__((packed));

struct tag_framebuffer_common {
    uint32_t type;
    uint32_t size;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;

    uint8_t framebuffer_type;
    uint16_t reserved;
} __attribute__((packed));

struct tag_framebuffer {
    struct tag_framebuffer_common common;

    union {
        struct {
            uint16_t framebuffer_palette_num_colors;
            struct color framebuffer_palette[0];
        } __attribute__((packed));
        struct {
            uint8_t framebuffer_red_field_position;
            uint8_t framebuffer_red_mask_size;
            uint8_t framebuffer_green_field_position;
            uint8_t framebuffer_green_mask_size;
            uint8_t framebuffer_blue_field_position;
            uint8_t framebuffer_blue_mask_size;
        } __attribute__((packed));
    };
} __attribute__((packed));

struct tag_elf_sections {
    uint32_t type;
    uint32_t size;
    uint32_t num;
    uint32_t entsize;
    uint32_t shndx;
    char sections[0];
} __attribute__((packed));

struct tag_apm {
    uint32_t type;
    uint32_t size;
    uint16_t version;
    uint16_t cseg;
    uint32_t offset;
    uint16_t cseg_16;
    uint16_t dseg;
    uint16_t flags;
    uint16_t cseg_len;
    uint16_t cseg_16_len;
    uint16_t dseg_len;
} __attribute__((packed));

struct tag_efi32 {
    uint32_t type;
    uint32_t size;
    uint32_t pointer;
} __attribute__((packed));

struct tag_efi64 {
    uint32_t type;
    uint32_t size;
    uint64_t pointer;
} __attribute__((packed));

struct tag_smbios {
    uint32_t type;
    uint32_t size;
    uint8_t major;
    uint8_t minor;
    uint8_t reserved[6];
    uint8_t tables[0];
} __attribute__((packed));

struct tag_old_acpi {
    uint32_t type;
    uint32_t size;
    uint8_t rsdp[0];
} __attribute__((packed));

struct tag_new_acpi {
    uint32_t type;
    uint32_t size;
    uint8_t rsdp[0];
} __attribute__((packed));

struct tag_network {
    uint32_t type;
    uint32_t size;
    uint8_t dhcpack[0];
} __attribute__((packed));

struct tag_efi_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t descr_size;
    uint32_t descr_vers;
    uint8_t efi_mmap[0];
} __attribute__((packed));

struct tag_efi32_ih {
    uint32_t type;
    uint32_t size;
    uint32_t pointer;
} __attribute__((packed));

struct tag_efi64_ih {
    uint32_t type;
    uint32_t size;
    uint64_t pointer;
} __attribute__((packed));

struct tag_load_base_addr {
    uint32_t type;
    uint32_t size;
    uint32_t load_base_addr;
} __attribute__((packed));

#endif