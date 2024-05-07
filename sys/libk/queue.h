#ifndef LIBK_QUEUE_H
#define LIBK_QUEUE_H

struct _queue_item {
    void *data;
    struct _queue_item *next;
};

struct queue {
    struct _queue_item *head;
    struct _queue_item *tail;
};

int
queue_enqueue(struct queue *queue, void *data);

void *
queue_dequeue(struct queue *queue);

int
queue_remove(struct queue *queue, void *data);

void
queue_destroy(struct queue *queue);

#endif