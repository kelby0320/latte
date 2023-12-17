#include "task/loader.h"

#include "errno.h"
#include "fs/fs.h"
#include "libk/kheap.h"
#include "mem/vm.h"
#include "task/elf.h"

#include <stdint.h>

static int
loader_map_program_header(struct elf_img_desc *elf_img_desc, struct vm_area *vm_area, int idx)
{
    struct elf32_phdr *phdr = &elf_img_desc->elf_pheaders[idx];

    if (phdr->p_type != PT_LOAD) {
        return 0; // Skip loading
    }

    size_t segment_offset = phdr->p_offset;
    size_t segment_size = phdr->p_memsz;

    // Ensure segment size aligned with the page size
    if (segment_size % VM_PAGE_SIZE) {
        segment_size += VM_PAGE_SIZE - (segment_size % VM_PAGE_SIZE);
    }

    if (elf_img_desc->num_seg_allocs == ELF_IMG_DESC_MAX_PHEADERS) {
        return -ENOMEM;
    }

    void *segment = kzalloc(segment_size);
    if (!segment) {
        return -ENOMEM;
    }

    elf_img_desc->segment_allocations[elf_img_desc->num_seg_allocs] = segment;
    elf_img_desc->num_seg_allocs++;

    int res = fseek(elf_img_desc->fd, segment_offset, SEEK_SET);
    if (res < 0) {
        res = -EIO;
        goto err_out;
    }

    res = fread(elf_img_desc->fd, segment, phdr->p_filesz);
    if (res < 0) {
        res = -EIO;
        goto err_out;
    }

    uint8_t flags = VM_PAGE_PRESENT | VM_PAGE_USER;
    if (phdr->p_flags & PF_W) {
        flags |= VM_PAGE_WRITABLE;
    }

    return vm_area_map_to(vm_area, phdr->p_vaddr, segment, segment + segment_size, flags);

err_out:
    kfree(segment);
    return res;
}

static int
loader_read_program_headers(struct elf_img_desc *img_desc)
{
    int pheader_start = img_desc->elf_header.e_ehsize;
    int res = fseek(img_desc->fd, pheader_start, SEEK_SET);
    if (res < 0) {
        return -EIO;
    }

    int num_pheaders = img_desc->elf_header.e_phnum;
    if (num_pheaders >= ELF_IMG_DESC_MAX_PHEADERS) {
        return -EINVAL;
    }

    res = fread(img_desc->fd, &img_desc->elf_pheaders, (sizeof(struct elf32_phdr) * num_pheaders));
    if (res < 0) {
        return -EIO;
    }

    img_desc->num_pheaders = num_pheaders;

    return 0;
}

static int
loader_read_elf_header(struct elf_img_desc *img_desc)
{
    int res = fread(img_desc->fd, &img_desc->elf_header, sizeof(struct elf32_ehdr));
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
loader_init_image(struct elf_img_desc *img_desc, const char *filename)
{
    int fd = fopen(filename, "r");
    if (fd < 0) {
        return -EIO;
    }

    img_desc->fd = fd;

    int res = loader_read_elf_header(img_desc);
    if (res < 0) {
        goto err_out;
    }

    res = loader_read_program_headers(img_desc);
    if (res < 0) {
        goto err_out;
    }

    img_desc->num_seg_allocs = 0;

    return 0;

err_out:
    fclose(fd);
    img_desc->fd = -1;

    return res;
}

void
loader_free_image(struct elf_img_desc *img_desc)
{
    fclose(img_desc->fd);
    img_desc->fd = -1;
}

int
loader_map_image(struct elf_img_desc *img_desc, struct vm_area *vm_area)
{
    for (int i = 0; i < img_desc->num_pheaders; i++) {
        int res = loader_map_program_header(img_desc, vm_area, i);
        if (res < 0) {
            return res;
        }
    }

    return 0;
}