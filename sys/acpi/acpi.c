#include "acpi.h"

#include "errno.h"
#include "libk/string.h"
#include "libk/memory.h"

#include <stddef.h>

#define ACPI_SEARCH_START_ADDR 0xe0000
#define ACPI_SEARCH_END_ADDR 0xfffff

struct acpi_rsdp *acpi_rsdp = NULL;
struct acpi_rsdt *acpi_rsdt = NULL;
struct acpi_fadt *acpi_fadt = NULL;
struct acpi_dsdt *acpi_dsdt = NULL;
struct acpi_mcfg *acpi_mcfg = NULL;

/**
 * acpi_table_map_t - Data type for acpi_table_map
 */
struct acpi_table_map_t {
    char signature[5];          /* 4 char signature + '\0' */
    void *table_ptr;
};

/**
 * acpi_table_map - A map of table signature to address where the table will be
 * saved.
 */
struct acpi_table_map_t acpi_table_map[] = {
    {
        .signature = "FACP",
        .table_ptr = &acpi_fadt
    },
    {
        .signature = "DSDT",
        .table_ptr = &acpi_dsdt
    },
    {
        .signature = "MCFG",
        .table_ptr = &acpi_mcfg
    }
};

#define ACPI_TABLE_MAP_SIZE (sizeof(acpi_table_map))

/**
 * find_rsdp - Search memory for the ACPI RSDP
 *
 * The ACPI RSDP should exist between 0xe0000 and 0xfffff. It is always aligned
 * to a 16 byte boundary and is identified by the first 8 bytes equaling the
 * string "RSD PTR ".  Note the trailing space and the fact there is no trailing
 * '\0'.
 *
 * Return: A pointer to a valid struct acpi_rsdp or NULL if not found
 */
static struct acpi_rsdp *
find_rsdp(void)
{
    void *rsdp_base = (void *)ACPI_SEARCH_START_ADDR;

    while (rsdp_base != (void *)ACPI_SEARCH_END_ADDR) {
        struct acpi_rsdp *rsdp = (struct acpi_rsdp *)rsdp_base;
        int cmp = memcmp(rsdp->signature, RSDP_SIGNATURE, 8);

        if (cmp != 0) {
            rsdp_base += 0xf; /* RSDP is on a 16 byte boundary */
            continue;
        }

        return (struct acpi_rsdp *)rsdp_base;
    }

    return NULL;
}

/**
 * match_table - Match a table header with a value from acpi_table_map
 *
 * Return: A matching table pointer or NULL
 */
static void *
match_table(void *table)
{
    char table_sig[5];
    strncpy(table_sig, table, 4);

    for (int i = 0; i < ACPI_TABLE_MAP_SIZE; i++) {
        const struct acpi_table_map_t *item = &acpi_table_map[i];
        if (strncmp(item->signature, table_sig, 4) == 0) {
            return item->table_ptr;
        }
    }

    return NULL;
}

/**
 * find_and_set_table_ptrs - Find and store pointers to ACPI tables
 *
 * Use the RSDT to find and store pointers to the following ACPI tables.
 *
 * Fixed ACPI Description Table (FADT)
 * Differentiated System Description Table (DSDT)
 * Memory Mapped Configuration (MCFG)
 *
 * Return: The number of tables found.
 */
static int
find_and_set_table_ptrs(struct acpi_rsdt *rsdt)
{
    size_t rsdt_len = rsdt_len(rsdt);
    int table_count = 0;

    for (size_t i = 0; i < rsdt_len; i++) {
        void *table = rsdt->entry + (i * sizeof(void *));
        void *table_ptr = match_table(table);
        if (table_ptr) {
            table_ptr = table;
            table_count++;
        }
    }

    return table_count;
}

int
acpi_init(void)
{
    acpi_rsdp = find_rsdp();

    if (!acpi_rsdp) {
        return -EEXIST;
    }

    acpi_rsdt = (struct acpi_rsdt*)acpi_rsdp->rsdt_addr;

    int table_count = find_and_set_table_ptrs(acpi_rsdt);

    return table_count;
}
