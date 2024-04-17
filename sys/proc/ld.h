#ifndef LD_H
#define LD_H

#include "proc/elf.h"

#include <stddef.h>

#define ELF_IMG_DESC_MAX_PHEADERS 8

/**
 * @brief ELF file image descriptor
 *
 */
struct elf_img_desc {
    // ELF file header
    struct elf32_ehdr elf_header;

    // List of ELF program headers
    struct elf32_phdr elf_pheaders[ELF_IMG_DESC_MAX_PHEADERS];

    // Number of ELF program headers
    int num_pheaders;

    // Memory segment allocations
    void *segment_allocations[ELF_IMG_DESC_MAX_PHEADERS];

    // Number of segment allocations
    int num_seg_allocs;

    // ELF file descriptor
    int fd;
};

struct process;
struct vm_area;

/**
 * @brief Initialize an ELF image from a file
 *
 * @param img_desc  Pointer to ELF image descriptor
 * @param filename  Name of the file
 * @return int      Status code
 */
int
ld_init_image(struct elf_img_desc *img_desc, const char *filename);

/**
 * @brief Free resources associated with an ELF image
 *
 * @param img_desc     Pointer to the ELF image descriptor
 */
void
ld_free_image(struct elf_img_desc *img_desc);

/**
 * @brief Map memory regions of an ELF image
 *
 * @param img_desc  Pointer to an ELF image descriptor
 * @param vm_area   Pointer to a VM area
 * @return int      Status code
 */
int
ld_map_image(struct elf_img_desc *img_desc, struct vm_area *vm_area);

#endif