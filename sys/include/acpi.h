#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>

#define RSDP_SIGNATURE "RSD PTR "
#define RSDT_SIGNATURE "RSDT"

/**
 * struct acpi_rsdp - Root System Description Pointer structure
 *
 * Note that last 4 fields starting with length are only valid for ACPI > 1.0.
 */
struct acpi_rsdp {
    char signature[8];          /* Signature - "RSD PTR " */
    uint8_t checksum;           /* Checksum defined by ACPI 1.0 */
    char oemid[6];              /* OEM supplied string */
    uint8_t revision;           /* ACPI revision number */
    void *rsdt_addr;            /* RSDP physical address */
    uint32_t length;            /* Length of the entire table */
    void *xsdt_addr;            /* XSDT physical address */
    uint8_t ext_checksum;       /* Checksum for the entire table */
    char reserved[3];           /* Reserved bytes */
};

/**
 * struct acpi_rsdt - Root System Description Table structure
 *
 * Note that oem_tableid should match the oem_tableid value in the FADT table.
 */
struct acpi_rsdt {
    char signature[4];          /* Signature - "RSDT" */
    uint32_t length;            /* Length of the table */
    uint8_t revision;           /* 1 */
    uint8_t checksum;           /* The entire table must sum to 0 */
    char oemid[6];              /* OEM supplied string */
    char oem_tableid[8];        /* Manufacture model id */
    uint32_t oem_revid;         /* OEM revision of oem_tableid */
    uint32_t creator_id;        /* Id of the vendor that created the table */
    uint32_t creator_rev;       /* Revision of utility that created the table */
    void *entry;                /* Array of physical addresses */
};

/**
 * struct acpi_fadt - Fixed ACPI Description Table
 *
 * This table defines various fixed hardware information vital to an ACPI
 * compatible OS.
 */
struct acpi_fadt {
    char signature[4];          /* Signature - "FACP" */
    uint32_t length;            /* Length of the table */
    uint8_t major_version;      /* FADT Version */
    uint8_t checksum;           /* The entire table must sum to 0 */
    char oemid[6];              /* OEM supplied string */
    char oem_tableid[8];        /* Manufacture model id */
    uint32_t oem_revid;         /* OEM revision of oem_tableid */
    uint32_t creator_id;        /* Id of the vendor that created the table */
    uint32_t creator_rev;       /* Revision of utility that created the table */
    void *fwcntl;               /* Physical address of FACS */
    void *dtst;                 /* Physical address of DSDT */
};

/**
 * struct acpi_dsdt - Differentiated System Description Table
 *
 * This table is part of the system fixed description. It consists of a system
 * description table followed by n bytes of data in Definition Block format.
 */
struct acpi_dsdt {
    char signature[4];          /* Signature - "DSDT" */
    uint32_t length;            /* Length of the table */
    uint8_t revision;           /* 2 */
    uint8_t checksum;           /* The entire table must sum to 0 */
    char oemid[6];              /* OEM supplied string */
    char oem_tableid[8];        /* Manufacture model id */
    uint32_t oem_revid;         /* OEM revision of oem_tableid */
    uint32_t creator_id;        /* Id of the vendor that created the table */
    uint32_t creator_rev;       /* Revision of utility that created the table */
    void *def_block;            /* n bytes of AML code */
};

struct acpi_mcfg {
    char signature[4];          /* Signature - "MCFG" */
    uint32_t length;            /* Length of the table */
    uint8_t revision;           /* 2 */
    uint8_t checksum;           /* The entire table must sum to 0 */
    char oemid[6];              /* OEM supplied string */
    char oem_tableid[8];        /* Manufacture model id */
    uint32_t oem_revid;         /* OEM revision of oem_tableid */
    uint32_t creator_id;        /* Id of the vendor that created the table */
    uint32_t creator_rev;       /* Revision of utility that created the table */
    char reserved[8];           /* Reserved */
    void *cfg_base_structs;     /* List of configuration base address allocation
                                   structures. */
};


#define rsdt_len(rsdt) \
    (rsdt->length - (sizeof(struct acpi_rsdt) - sizeof(void*)))

/**
 * acpi_init - Intialize the ACPI subsystem
 *
 * Return: The number of tables found
 */
int acpi_init(void);

#endif

