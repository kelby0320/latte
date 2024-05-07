#ifndef LIBK_LIST_H
#define LIBK_LIST_H

#define for_each_in_list(T, head, list, item)                                                      \
    struct list_item *list;                                                                        \
    T item;                                                                                        \
    for (list = head, item = list->data; list != NULL; list = list->next, item = list->data)

struct list_item {
    void *data;
    struct list_item *next;
};

/**
 * @brief   Add an item to the front of list
 *
 * @param head  The head of the list
 * @param data  The data to append
 * @return int
 */
int
list_push_front(struct list_item **head, void *data);

/**
 * @brief   Remove an item from the front of list
 *
 * @param head      The head of the list
 * @return void*    The data
 */
void *
list_pop_front(struct list_item **head);

/**
 * @brief   Get the front of the list
 *
 * @param head      The head of the list
 * @return void*    The data
 */
void *
list_front(struct list_item *head);

/**
 * @brief   Remove an item from the list
 *
 * @param head  The head of the list
 * @param data  The data to remove
 * @return int
 */
int
list_remove(struct list_item **head, void *data);

/**
 * @brief   Destroy the list
 *
 * Note: This will not free the interal data pointer.  It is the caller's
 * responsibility to free all memory pointed to by list items.
 *
 * @param head  The head of the list
 */
void
list_destroy(struct list_item *head);

#endif