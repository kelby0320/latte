#include "task/elf.h"

#include "config.h"
#include "libk/memory.h"

const char ELF_SIGNATURE[] = {0x7f, 'E', 'L', 'F'};

int
elf32_is_valid_signature(struct elf32_ehdr *ehdr)
{
    return memcmp(&ehdr->e_ident, ELF_SIGNATURE, sizeof(ELF_SIGNATURE)) == 0;
}

int
elf32_is_valid_class(struct elf32_ehdr *ehdr)
{
    return ehdr->e_ident[EI_CLASS] == EI_CLASSNONE || ehdr->e_ident[EI_CLASS] == EI_CLASS32;
}

int
elf32_is_valid_encoding(struct elf32_ehdr *ehdr)
{
    return ehdr->e_ident[EI_DATA] == EI_DATANONE || ehdr->e_ident[EI_DATA] == EI_DATA2LSB;
}

int
elf32_is_valid_version(struct elf32_ehdr *ehdr)
{
    return ehdr->e_version == EV_CURRENT;
}

int
elf32_is_exec(struct elf32_ehdr *ehdr)
{
    return ehdr->e_type == ET_EXEC && ehdr->e_entry == LATTE_TASK_LOAD_VIRT_ADDR;
}

int
elf32_is_valid(struct elf32_ehdr *ehdr)
{
    return (elf32_is_valid_signature(ehdr) && elf32_is_valid_class(ehdr) &&
            elf32_is_valid_encoding(ehdr) && elf32_is_valid_version(ehdr) && elf32_is_exec(ehdr));
}