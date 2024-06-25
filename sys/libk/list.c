#include "libk/list.h"

#include "errno.h"
#include "libk/alloc.h"

int
list_push_front(struct list_item **head, void *data)
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
list_push_back(struct list_item **head, void *data)
{
    struct list_item *new_item = kzalloc(sizeof(struct list_item));
    if (!new_item) {
        return -ENOMEM;
    }

    new_item->data = data;
    new_item->next = NULL;

    if (!*head) {
        *head = new_item;
        return 0;
    }

    struct list_item *item = *head;
    while (item->next) {
        item = item->next;
    }

    item->next = new_item;

    return 0;
}

void *
list_pop_front(struct list_item **head)
{
    if (!*head) {
        return NULL;
    }

    struct list_item *item = *head;
    void *data = item->data;
    *head = item->next;

    kfree(item);

    return data;
}

void *
list_front(struct list_item *head)
{
    if (!head) {
        return NULL;
    }

    return head->data;
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

bool
list_empty(struct list_item *head)
{
    return head == NULL;
}

void
list_destroy(struct list_item *head)
{
    for (struct list_item *item = head; item != NULL; item = item->next) {
        kfree(item);
    }
}