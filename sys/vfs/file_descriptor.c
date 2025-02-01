#include "file_descriptor.h"

#include "config.h"
#include "errno.h"
#include "libk/alloc.h"
#include "libk/memory.h"

struct file_descriptor *
    global_file_descriptor_table[LATTE_GLOBAL_FILE_DESCRITPOR_TABLE_SIZE] = {0};

struct file_descriptor *
file_descriptor_get(int fd)
{
    if (fd < 0 || fd >= LATTE_GLOBAL_FILE_DESCRITPOR_TABLE_SIZE) {
        return NULL;
    }

    return global_file_descriptor_table[fd];
}

int
file_descriptor_get_new(struct file_descriptor **desc_out)
{
    for (int i = 0; i < LATTE_GLOBAL_FILE_DESCRITPOR_TABLE_SIZE; i++) {
        if (global_file_descriptor_table[i] == 0) {
            struct file_descriptor *desc =
                kzalloc(sizeof(struct file_descriptor));
            if (!desc) {
                return -ENOMEM;
            }

            desc->index = i;
            global_file_descriptor_table[i] = desc;
            *desc_out = desc;
            return 0;
        }
    }

    return -ENFILE;
}

void
file_descriptor_free(struct file_descriptor *descriptor)
{
    global_file_descriptor_table[descriptor->index] = 0;
    kfree(descriptor);
}
