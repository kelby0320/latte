#ifndef LD_H
#define LD_H

#include "elf.h"

#include <stddef.h>

#define ELF_IMG_DESC_MAX_PHEADERS 8

struct process;

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
 * @brief Map memory regions of an ELF image to a process
 *
 * @param img_desc  Pointer to a process
 * @return int      Status code
 */
int
ld_map_image_to_process(struct process *process);

#endif
