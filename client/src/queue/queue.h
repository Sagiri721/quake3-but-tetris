/**
 * @file        queue.h
 * @brief       A queue implementation
 */

#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE_SIZE 100

// Queue data structure, max size should be enough
// as long as it is being dequeued (i hope)
typedef struct {
    int buffer[MAX_QUEUE_SIZE];
    int head;
    int size;
} queue;

void queue_init(queue* q);

char is_empty(queue* q);
char is_full(queue* q);

// Return false on fail
char enqueue(queue* q, int value);
// Return false on fail
char dequeue(queue* q, int* value);

#endif