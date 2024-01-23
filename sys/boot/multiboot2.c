/**
 * @file multiboot2.c
 * @author Kelby Madal-Hellmuth (kelby0320@gmail.com)
 * @brief Multiboot utility functions
 * @version 0.1
 * @date 2023-12-27
 *
 * @copyright Copyright (c) 2023
 *
 * @license SPDX-License-Identifier: MIT
 */
#include "boot/multiboot2.h"

#include "errno.h"

#include "stddef.h"

static void *
multiboot2_find_tag(void *addr, unsigned int tag_no)
{
    struct multiboot2_tag *start_tag = addr + 8;

    for (struct multiboot2_tag *tag = start_tag; tag->type != MULTIBOOT2_TAG_TYPE_END;
         tag = (struct multiboot2_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
        if (tag->type == tag_no) {
            return tag;
        }
    }

    return NULL;
}

int
multiboot2_verify_magic_number(unsigned long magic)
{
    return magic == MULTIBOOT2_BOOTLOADER_MAGIC;
}

int
multiboot2_get_boot_device(void *addr, uint32_t *biosdev, uint32_t *part_no)
{
    struct multiboot2_tag_bootdev *bootdev = multiboot2_find_tag(addr, MULTIBOOT2_TAG_TYPE_BOOTDEV);
    if (!bootdev) {
        return -ENOENT;
    }

    *biosdev = bootdev->biosdev;
    *part_no = bootdev->part;

    return 0;
}