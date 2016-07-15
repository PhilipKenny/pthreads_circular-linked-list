#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct
{
    //Number of bytes per element
    int size;
    //Number of elements in the queue
    int total_length;
    //Current length of the queue
    int current_length;
    
    void * head;  
    void * tail;
    void * start;
    void * end;
    
    //Mutex guaranteeing the thread safe access to the queue
    pthread_mutex_t mutex_queue;
   
    
    //Condition variables indicating that the queue is full or empty, respectively
    pthread_cond_t cond_full;
    pthread_cond_t cond_empty;
    
} queue_t;

void queue_init(queue_t * queuep, int size_of_elements, int num_of_elements);
void queue_push(queue_t * queuep, void * element);
void queue_pop(queue_t * queuep, void * element);
void queue_destroy(queue_t * queuep);

#endif
