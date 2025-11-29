/**
 * @file        queue.h
 * @brief       A queue implementation
 */

#include "queue.h"
#include <stdio.h>

void queue_init(queue *q) {

    q->head = 0;
    q->size = 0;
}

char is_empty(queue *q) {
    return q->size == 0;
}

char is_full(queue *q) {
    return q->size == MAX_QUEUE_SIZE;
}

char enqueue(queue* q, int value) {
    
    if (is_full(q)) {
        fprintf(stderr, "Queue %p is full, cannot enqueue value %d\n", q, value);
        return 0;
    }

    int tail = (q->head + q->size) % MAX_QUEUE_SIZE;
    q->buffer[tail] = value;
    q->size++;
    return 1;
}

char dequeue(queue* q, int* value) {
    if (is_empty(q)) {
        fprintf(stderr, "Queue %p is empty, cannot dequeue value\n", q);
        return 0;
    }

    *value = q->buffer[q->head];
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    q->size--;
    return 1;
}