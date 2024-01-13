#ifndef CONFIG_H
#define CONFIG_H

#define LATTE_TOTAL_GDT_SEGMENTS  6
#define LATTE_KERNEL_CODE_SEGMENT 0x08
#define LATTE_KERNEL_DATA_SEGMENT 0x10
#define LATTE_USER_CODE_SEGMENT   0x18 | 0x3
#define LATTE_USER_DATA_SEGMENT   0x20 | 0x3
#define LATTE_TSS_SEGMENT         0x28

#define LATTE_TOTAL_IDT_ENTRIES 256

#define LATTE_TSS_ESP 0x600000

#define LATTE_HEAP_MIN_BLOCK_SIZE 4096
#define LATTE_HEAP_MAX_ORDER      10
#define LATTE_HEAP_LARGE_BLOCKS   128
#define LATTE_HEAP_MAX_BLOCK_SIZE (LATTE_HEAP_MIN_BLOCK_SIZE * (1 << LATTE_HEAP_MAX_ORDER))
#define LATTE_HEAP_ADDRESS        0x01000000

#define LATTE_SECTOR_SIZE      512
#define LATTE_MAX_DISKS        16
#define LATTE_DISK_ID_MAX_SIZE 8

#define LATTE_DEVICE_NAME_MAX_SIZE 64
#define LATTE_BUS_NAME_MAX_SIZE    64
#define LATTE_DRIVER_NAME_MAX_SIZE 64

#define LATTE_MAX_BUSES   16
#define LATTE_MAX_DEVICES 32

#define LATTE_MAX_FILESYSTEMS                   16
#define LATTE_GLOBAL_FILE_DESCRITPOR_TABLE_SIZE 4096
#define LATTE_MAX_MOUNTPOINTS                   16

#define LATTE_MAX_PATH_LEN       255
#define LATTE_MAX_PATH_ELEM_SIZE 64

#define LATTE_PROCESS_MAX_PROCESSES   1024
#define LATTE_PROCESS_MAX_ALLOCATIONS 1024
#define LATTE_PROCESS_MAX_TASKS       32

#define LATTE_TASK_LOAD_VIRT_ADDR         0x400000
#define LATTE_TASK_STACK_SIZE             (4096 * 16)
#define LATTE_TASK_STACK_VIRT_ADDR_TOP    0x3FF000
#define LATTE_TASK_STACK_VIRT_ADDR_BOTTOM LATTE_TASK_STACK_VIRT_ADDR_TOP - LATTE_TASK_STACK_SIZE
#define LATTE_TASK_MAX_TASKS              (LATTE_PROCESS_MAX_PROCESSES * 4)

#endif
