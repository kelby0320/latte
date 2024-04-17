#include "proc/ld.h"

#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "mm/kalloc.h"
#include "mm/paging/paging.h"
#include "mm/vm.h"
#include "proc/elf.h"
#include "proc/process.h"
#include "vfs/vfs.h"

#include <stdint.h>

/**
 * @brief   Load a program header into memory
 *
 * @param elf_img_desc  ELF image descriptor
 * @param vm_area       VM area
 * @param idx           Index of the program header
 * @return int          Status code
 */
static int
ld_map_program_header(struct process *process, int idx)
{
    // Note: Allocated segment size may be larger than requested size
    // As a result, subsequently allocated segments may overlap
    //
    // FIXME

    struct elf32_phdr *phdr = &process->elf_img_desc.elf_pheaders[idx];

    if (phdr->p_type != PT_LOAD) {
        return 0; // Skip loading
    }

    size_t elf_segment_size = phdr->p_memsz;

    // Ensure segment size aligned with the page size
    if (elf_segment_size % PAGING_PAGE_SIZE) {
        elf_segment_size += PAGING_PAGE_SIZE - (elf_segment_size % PAGING_PAGE_SIZE);
    }

    int order = kalloc_size_to_order(elf_segment_size);

    void *segment = kalloc_get_phys_pages(order);
    if (!segment) {
        return -ENOMEM;
    }

    size_t segment_size = kalloc_order_to_size(order);

    int res = vfs_seek(process->elf_img_desc.fd, phdr->p_offset, SEEK_SET);
    if (res < 0) {
        res = -EIO;
        goto err_out;
    }

    res = vfs_read(process->elf_img_desc.fd, segment, phdr->p_filesz);
    if (res < 0) {
        res = -EIO;
        goto err_out;
    }

    uint8_t flags = PAGING_PAGE_PRESENT | PAGING_PAGE_USER;
    if (phdr->p_flags & PF_W) {
        flags |= PAGING_PAGE_WRITABLE;
    }

    res = vm_area_map_pages_to(process->vm_area, (void *)phdr->p_vaddr, segment,
                               segment + segment_size, flags);
    if (res < 0) {
        goto err_out;
    }

    struct process_allocation segment_allocation = {
        .type = PROCESS_ALLOCATION_PROGRAM_SEGMENT, .addr = segment, .size = segment_size};

    list_item_append(process_allocation_t, &process->allocations, segment_allocation);

    return res;

err_out:
    kfree(segment);
    return res;
}

/**
 * @brief   Read the ELF program headers
 *
 * @param img_desc  ELF image descriptor
 * @return int      Status code
 */
static int
ld_read_program_headers(struct elf_img_desc *img_desc)
{
    int pheader_start = img_desc->elf_header.e_ehsize;
    int res = vfs_seek(img_desc->fd, pheader_start, SEEK_SET);
    if (res < 0) {
        return -EIO;
    }

    int num_pheaders = img_desc->elf_header.e_phnum;
    if (num_pheaders >= ELF_IMG_DESC_MAX_PHEADERS) {
        return -EINVAL;
    }

    res = vfs_read(img_desc->fd, (char *)&img_desc->elf_pheaders,
                   (sizeof(struct elf32_phdr) * num_pheaders));
    if (res < 0) {
        return -EIO;
    }

    img_desc->num_pheaders = num_pheaders;

    return 0;
}

/**
 * @brief   Read the ELF header
 *
 * @param img_desc  ELF image descriptor
 * @return int      Status code
 */
static int
ld_read_elf_header(struct elf_img_desc *img_desc)
{
    int res = vfs_read(img_desc->fd, (char *)&img_desc->elf_header, sizeof(struct elf32_ehdr));
    if (res < 0) {
        return -EIO;
    }

    res = elf32_is_valid(&img_desc->elf_header);
    if (res < 0) {
        return res;
    }

    return 0;
}

int
ld_init_image(struct elf_img_desc *img_desc, const char *filename)
{
    int fd = vfs_open(filename, "r");
    if (fd < 0) {
        return -EIO;
    }

    img_desc->fd = fd;

    int res = ld_read_elf_header(img_desc);
    if (res < 0) {
        goto err_out;
    }

    res = ld_read_program_headers(img_desc);
    if (res < 0) {
        goto err_out;
    }

    return 0;

err_out:
    vfs_close(fd);
    img_desc->fd = -1;

    return res;
}

void
ld_free_image(struct elf_img_desc *img_desc)
{
    vfs_close(img_desc->fd);
    img_desc->fd = -1;
}

int
ld_map_image_to_process(struct process *process)
{
    for (int i = 0; i < process->elf_img_desc.num_pheaders; i++) {
        int res = ld_map_program_header(process, i);
        if (res < 0) {
            return res;
        }
    }

    return 0;
}