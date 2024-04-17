#ifndef LIBK_LIST_H
#define LIBK_LIST_H

#include "errno.h"
#include "libk/alloc.h"

/**
 * @brief   List prototypes for use in header files
 *
 */
#define LIST_ITEM_TYPE_PROTO(T)                                                                    \
    struct list_item_##T;                                                                          \
    int list_item_append_##T(struct list_item_##T **head, T data);                                 \
    void list_item_destroy_##T(struct list_item_##T *head);

/**
 * @brief   List definitions for use in source files
 *
 * Do not use this macro in header files. You will get a load
 * of linker errors complaining about multiple definitions.
 *
 */
#define LIST_ITEM_TYPE_DEF(T)                                                                      \
    struct list_item_##T {                                                                         \
        T data;                                                                                    \
        struct list_item_##T *next;                                                                \
    };                                                                                             \
                                                                                                   \
    int list_item_append_##T(struct list_item_##T **head, T data)                                  \
    {                                                                                              \
        struct list_item_##T *new_item = kzalloc(sizeof(struct list_item_##T));                    \
        if (!new_item) {                                                                           \
            return -ENOMEM;                                                                        \
        }                                                                                          \
                                                                                                   \
        new_item->data = data;                                                                     \
        new_item->next = *head;                                                                    \
        *head = new_item;                                                                          \
                                                                                                   \
        return 0;                                                                                  \
    }                                                                                              \
                                                                                                   \
    void list_item_destroy_##T(struct list_item_##T *head)                                         \
    {                                                                                              \
        for (struct list_item_##T *item = head; item != NULL; item = item->next) {                 \
            kfree(item);                                                                           \
        }                                                                                          \
    }

#define LIST(T) struct list_item_##T *

#define for_each_in_list(T, head, item)                                                            \
    for (struct list_item_##T *item = head; item != NULL; item = item->next)

#define list_item_append(T, head, data) list_item_append_##T(head, data)

#define list_item_remove(T, head, data) list_item_remove_##T(head, data)

#define list_item_destroy(T, head) list_item_destroy_##T(head)

#endif