#ifndef ELF_H
#define ELF_H

#include <stdbool.h>
#include <stdint.h>

#define EI_NIDENT       16
#define EI_CLASS        4

#define EI_CLASSNONE    0
#define EI_CLASS32      1
#define EI_CLASS64      2

#define EI_DATA         5
#define EI_DATANONE     0
#define EI_DATA2LSB     1
#define EI_DATA2MSB     2

#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2
#define ET_DYN          3
#define ET_CORE         4

#define EM_M32          1
#define EM_SPARC        2
#define EM_386          3
#define EM_68K          4
#define EM_88K          5
#define EM_860          6
#define EM_MIPS         7
#define EM_MIPS_RS4_BE  8
#define EM_M32          10

#define EV_NONE         0
#define EV_CURRENT      1

#define PT_NULL         0
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6

#define PF_X            0x1     // Program flags execute permission
#define PF_W            0x2     // Program flags write permission
#define PF_R            0x4     // Program flags read permission

typedef uint32_t elf32_addr;
typedef uint16_t elf32_half;
typedef uint32_t elf32_off;
typedef int32_t elf32_sword;
typedef uint32_t elf32_word;

struct elf32_ehdr {
    unsigned char   e_ident[EI_NIDENT]; /* ELF marker and machine independent data */
    elf32_half      e_type;             /* ELF file type */
    elf32_half      e_machine;          /* ELF machine architecture */
    elf32_word      e_version;          /* ELF version */
    elf32_addr      e_entry;            /* Virtual address program entry point */
    elf32_off       e_phoff;            /* Program header table file offset */
    elf32_off       e_shoff;            /* Section header table file offset */
    elf32_word      e_flags;            /* Processor specific flags */
    elf32_half      e_ehsize;           /* ELF header size in bytes */
    elf32_half      e_phentsize;        /* Program header table entry size in bytes */
    elf32_half      e_phnum;            /* Number of program header table entries */
    elf32_half      e_shentsize;        /* Section header table entry size in bytes */
    elf32_half      e_shnum;            /* Number of section header table entries */
    elf32_half      e_shstrndx;         /* Section header table entry of the section name string table */
} __attribute__((packed));

struct elf32_phdr {
    elf32_word      p_type;             /* Segment type */
    elf32_off       p_offset;           /* File offset of segment */
    elf32_addr      p_vaddr;            /* Segment virtual address */
    elf32_addr      p_paddr;            /* Segment physical address */
    elf32_word      p_filesz;           /* Segment size in file */
    elf32_word      p_memsz;            /* Segment size in memory */
    elf32_word      p_flags;            /* Segment flags */
    elf32_word      p_align;            /* Alignment */
} __attribute__((packed));

struct elf32_shdr {
    elf32_word      sh_name;            /* Section name (index into section header string table)*/
    elf32_word      sh_type;            /* Section type */
    elf32_word      sh_flags;           /* Section flags */
    elf32_addr      sh_addr;            /* Section memory address */
    elf32_off       sh_off;             /* Section file offset */
    elf32_word      sh_size;            /* Section size in bytes */
    elf32_word      sh_link;            /* Section header table index link */
    elf32_word      sh_info;            /* Section information */
    elf32_word      sh_addralign;       /* Section address alignment */
    elf32_word      sh_entsize;         /* Section table entry size */
} __attribute__((packed));

/**
 * @brief Validate the ELF header signature
 * 
 * @param ehdr      Ponter to the ELF header structure
 * @return int      Status code
 */
int
elf32_is_valid_signature(struct elf32_ehdr *ehdr);

/**
 * @brief Validate the ELF header class
 * 
 * @param ehdr      Ponter to the ELF header structure
 * @return int      Status code
 */
int
elf32_is_valid_class(struct elf32_ehdr *ehdr);

/**
 * @brief Validate the ELF header encoding
 * 
 * @param ehdr      Ponter to the ELF header structure
 * @return int      Status code
 */
int
elf32_is_valid_encoding(struct elf32_ehdr *ehdr);

/**
 * @brief Validate the ELF header version
 * 
 * @param ehdr      Ponter to the ELF header structure
 * @return int      Status code
 */
int
elf32_is_valid_version(struct elf32_ehdr *ehdr);

/**
 * @brief Validate the ELF header is executable
 * 
 * @param ehdr      Ponter to the ELF header structure
 * @return int      Status code
 */
int
elf32_is_exec(struct elf32_ehdr *ehdr);

/**
 * @brief Validate an ELF header
 * 
 * @param ehdr      Pointer to the ELF header structure
 * @return int      Status code
 */
int
elf32_is_valid(struct elf32_ehdr *ehdr);

#endif