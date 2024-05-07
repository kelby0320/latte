#include "libk/queue.h"

#include "errno.h"
#include "libk/alloc.h"

int
queue_enqueue(struct queue *queue, void *data)
{
    struct _queue_item *new_item = kzalloc(sizeof(struct _queue_item));
    if (!new_item) {
        return -ENOMEM;
    }

    new_item->data = data;
    new_item->next = NULL;

    if (!queue->head) {
        queue->head = new_item;
        queue->tail = new_item;
    } else {
        queue->tail->next = new_item;
        queue->tail = new_item;
    }

    return 0;
}

void *
queue_dequeue(struct queue *queue)
{
    if (!queue->head) {
        return NULL;
    }

    struct _queue_item *item = queue->head;
    void *data = item->data;
    queue->head = item->next;

    if (queue->tail == item) {
        queue->tail = NULL;
    }

    kfree(item);

    return data;
}

int
queue_remove(struct queue *queue, void *data)
{
    struct _queue_item *prev = NULL;
    for (struct _queue_item *item = queue->head; item != NULL; item = item->next) {
        if (item->data == data) {
            if (prev) {
                prev->next = item->next;
            } else {
                queue->head = item->next;
            }

            if (queue->tail == item) {
                queue->tail = prev;
            }

            kfree(item);
            return 0;
        }

        prev = item;
    }

    return -ENOENT;
}

void
queue_destroy(struct queue *queue)
{
    for (struct _queue_item *item = queue->head; item != NULL; item = item->next) {
        kfree(item);
    }
}
