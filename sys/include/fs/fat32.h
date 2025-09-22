#ifndef FAT32_H
#define FAT32_H

#include "fs.h"

#ifdef __cplusplus
extern "C" {
#endif
struct filesystem *
fat32_init();

#ifdef __cplusplus
}
#endif
#endif
