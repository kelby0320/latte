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

int
multiboot2_verify_magic_number(unsigned long magic)
{
    return magic == MULTIBOOT2_BOOTLOADER_MAGIC;
}