/*
 * queue.h
 *
 *  Created on: Nov 10, 2022
 *      Author: kihong.park
 */

#ifndef PROJECT_5_QUEUE_H_
#define PROJECT_5_QUEUE_H_

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// A structure to represent a queue
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};

struct Queue* createQueue(unsigned capacity);
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
void enqueue(struct Queue* queue, int item);
int dequeue(struct Queue* queue);
int front(struct Queue* queue);
int rear(struct Queue* queue);

#endif /* PROJECT_5_QUEUE_H_ */
