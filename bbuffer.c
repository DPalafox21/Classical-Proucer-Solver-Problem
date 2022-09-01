/*
 * The code is not part of the real application, and just used to 
 * illustrate the bounded-buffer problem using Semaphore and/or mutexes. 
 * Detailed requirements please refer to the assignment documentation.
 * 
*/

#include <stdio.h> 
#include <stdlib.h> 
#include "bbuffer.h"

void initilization()
{
    /* TODO: intialize golbal variables if you need, such as semaphore, mutex and etc. Leave it empyty if you
     * do not need it.
     */     
    pthread_mutex_init(&lock, NULL);
    items_in_buffer = 0;
     
}

/* *
 * insert_item - thread safe(?) function to insert items to the bounded buffer
 * @param item the value to be inserted
 * @return 0 in case of sucess -1 otherwise
 */
  
int insert_item(int item, long int id)
{
    /* TODO: Check and wait if the buffer is full. Ensure exclusive
     * access to the buffer and may use the existing code to insert an item.
     */   

    //lock system that way we can check our items in variable to ensure we can acces data
    pthread_mutex_lock(&lock);
    //if items in buffer is the buffer size then we have to wait to add more info to buffer 
    while (items_in_buffer == BUFFER_SIZE ) {
        //unlock to allow others to add to the buffer
        pthread_mutex_unlock(&lock);
        //sleep while others do their work
        sleep(rand() % 3);
        //lock our system again and try agian
        pthread_mutex_lock(&lock);
    }
    buffer.value[buffer.next_in] = item;
    
      
    printf("producer %ld: inserted item %d into buffer index %d\n", id, item, buffer.next_in);
       
    buffer.next_in = (buffer.next_in + 1) % BUFFER_SIZE;
    
    //we added data so we need to increase our buffer size
    items_in_buffer += 1;
    pthread_mutex_unlock(&lock);

    return 0;
}

/**
 * remove_item - thread safe(?) function to remove items to the bounded buffer
 * @param item the address of the variable that the removed value will be written
 * @return 0 in case of sucess -1 otherwise
 */
int remove_item(int *item, long int id)
{
    /* TODO: Check and wait if the buffer is empty. Ensure exclusive
     * access to the buffer and use the existing code to remove an item.
     */
    //lock system that way we can check our items in variable to ensure we can acces data
    pthread_mutex_lock(&lock);
    //if items in buffer is the 0 then we have to wait to remove more info from buffer 
    while (items_in_buffer == 0 ) {
        //unlock to allow others to add to the buffer
        pthread_mutex_unlock(&lock);
        //sleep while others do their work
        sleep(rand() % 3);
        //lock our system again and try agian
        pthread_mutex_lock(&lock);
    }

    

    *item = buffer.value[buffer.next_out];
    buffer.value[buffer.next_out] = -1;    
    
   
    printf("consumer %ld: removed item %d from buffer index %d\n", id, *item, buffer.next_out);
       
    buffer.next_out = (buffer.next_out + 1) % BUFFER_SIZE;
    //we took an item from the buffer and need to make that known
    items_in_buffer -= 1;
    //free our lock since we finished what we needed to do
    pthread_mutex_unlock(&lock);
    return 0;
}

/**
 * producer - will iterate PRODUCER_ITERATION times and call the corresponding
 * function to insert an integer to the bounded buffer
 * @param param an integer id of the producer used to distinguish between the
 * multiple producer threads
 * @return nothing
 */
void * producer(void *param)
{
    int item, i;
    long int id = (long int)param;

    printf("producer %d started!\n", id);
    i = PRODUCER_ITERATIONS;
    while (i--) {
	  sleep(rand() % 3);

	item = rand() % 10000;
	if (insert_item(item, id))
	    printf("Error while inserting to buffer\n");
    }

    pthread_exit(0);
}

/**
 * consumer - will iterate CONSUMER_ITERATION times and call the corresponding
 * function to remove an integer from the bounded buffer
 * @param param an integer id of the producer used to distinguish between the
 * multiple consumer threads
 * @return nothing
 */
void * consumer(void *param)
{
    int item, i;
    long int id = (long int)param;

    printf("consumer %d started!\n", id);
    i = CONSUMER_ITERATIONS;
    while (i--) {
	sleep(rand() % 3);

	if (remove_item(&item, id))
	    printf("Error while removing from buffer\n");
    }

    pthread_exit(0);
}
