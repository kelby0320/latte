#ifndef LIBK_LIST_H
#define LIBK_LIST_H

// list_for_each_entry(palloc, &process->allocations, struct process_allocation, list)

#define for_each_in_list(T, head, list, item)                                                      \
    struct list_item *list;                                                                        \
    T item;                                                                                        \
    for (list = head, item = list->data; list != NULL; list = list->next)

struct list_item {
    void *data;
    struct list_item *next;
};

/**
 * @brief   Append an item to the list
 *
 * @param head  The head of the list
 * @param data  The data to append
 * @return int
 */
int
list_append(struct list_item **head, void *data);

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
 * @param head  The head of the list
 */
void
list_destroy(struct list_item *head);

#endif