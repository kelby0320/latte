#ifndef CONFIG_H
#define CONFIG_H

#define LATTE_TOTAL_GDT_SEGMENTS 5

#define LATTE_HEAP_MIN_BLOCK_SIZE 4096
#define LATTE_HEAP_MAX_ORDER 10
#define LATTE_HEAP_LARGE_BLOCKS 128
#define LATTE_HEAP_MAX_BLOCK_SIZE (LATTE_HEAP_MIN_BLOCK_SIZE * (1 << LATTE_HEAP_MAX_ORDER))
#define LATTE_HEAP_ADDRESS 0x01000000

#define LATTE_MAX_DISKS 16

#endif
