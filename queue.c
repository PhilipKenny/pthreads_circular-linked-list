#include "queue.h"
#include <pthread.h>
#include <time.h>

#define TIME_TO_WAIT    5

// Initializing the queue
void queue_init (queue_t * queuep, int size_of_elements, int num_of_elements)
{
    // Size of the elements in the queue
    queuep->size = size_of_elements;
    queuep->total_length = num_of_elements - 1;
    queuep->current_length = 0;
    // Allocating memory for the queue
    queuep->start = malloc (size_of_elements * num_of_elements);
    if (NULL == queuep->start)
    {
        printf ("Not enough memory (or something else), yo\n");
        exit (EXIT_FAILURE);
    }
    // Initially head and tail of the queue point to starting point of the allocated memory.
    queuep->head = queuep->tail = queuep->start;
    queuep->end = queuep->start + (size_of_elements * num_of_elements);
    // The mutex is initialized.
    pthread_mutex_init (&queuep->mutex_queue, NULL);
    // The conditional variables are initialized.    
    pthread_cond_init (&queuep->cond_full, NULL);
    pthread_cond_init (&queuep->cond_empty, NULL);
}

//Method to store elements in the queue
void queue_push (queue_t * queuep, void *element)
{
    struct timespec wait_time = {0,0};
    //Lock the mutex
    pthread_mutex_lock (&queuep->mutex_queue);
    //Check if the queue is full
    while (!(queuep->total_length - queuep->current_length))
    {
        printf ("The queue is full.\n");
        wait_time.tv_sec = time(NULL) + TIME_TO_WAIT;      
        // In this case wait on the conditional variable and unlock the mutex.
        pthread_cond_timedwait (&queuep->cond_full, &queuep->mutex_queue, &wait_time);
    }
    // When queue is not full, store element.
    if (NULL == element)
    {
        queuep->tail = NULL;
    }
    else
    {
        memcpy (queuep->tail, element, queuep->size);
    }
    queuep->current_length++;
    // Broadcast to wake up threads that are waiting, because queue is empty.
    pthread_cond_broadcast (&queuep->cond_empty);
    // Shift tail
    queuep->tail += queuep->size;
    if (queuep->tail == queuep->end)
    {
        queuep->tail = queuep->start;
    }
    // Unlock the mutex
    pthread_mutex_unlock (&queuep->mutex_queue);
}


// Method to remove elements from the queue
void queue_pop (queue_t * queuep, void *element)
{
    struct timespec wait_time = {0,0};
    // Lock the mutex
    pthread_mutex_lock (&queuep->mutex_queue);
    // Check if queue is empty.   
    while (!queuep->current_length)
    {
        printf ("The queue is empty.\n");
        wait_time.tv_sec = time(NULL) + TIME_TO_WAIT; 
        pthread_cond_timedwait (&queuep->cond_empty, &queuep->mutex_queue, &wait_time);
    }
    // When queue is not empty, remove element.
    if (NULL == queuep->head)
    {
        element = NULL;
    }
    else
    {
        memcpy (element, queuep->head, queuep->size);
    }
    queuep->current_length--;
    // Broadcast to wake up threads that are waiting, because queue is full.
    pthread_cond_broadcast (&queuep->cond_full);
    // Shift head of queue
    queuep->head += queuep->size;
    if (queuep->head == queuep->end)
    {
        queuep->head = queuep->start;
    }
    // Unlock the mutex
    pthread_mutex_unlock (&queuep->mutex_queue);
}

// Destructor of the queue
void
queue_destroy (queue_t * queuep)
{
  free (queuep->start);
}
