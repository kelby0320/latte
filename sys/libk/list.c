#include "libk/list.h"

#include "errno.h"
#include "libk/alloc.h"

int
list_append(struct list_item **head, void *data)
{
    struct list_item *new_item = kzalloc(sizeof(struct list_item));
    if (!new_item) {
        return -ENOMEM;
    }

    new_item->data = data;
    new_item->next = *head;
    *head = new_item;

    return 0;
}

int
list_remove(struct list_item **head, void *data)
{
    struct list_item *prev = NULL;
    for (struct list_item *item = *head; item != NULL; item = item->next) {
        if (item->data == data) {
            if (prev) {
                prev->next = item->next;
            } else {
                *head = item->next;
            }

            kfree(item);

            return 0;
        }

        prev = item;
    }

    return -ENOENT;
}

void
list_destroy(struct list_item *head)
{
    for (struct list_item *item = head; item != NULL; item = item->next) {
        kfree(item);
    }
}