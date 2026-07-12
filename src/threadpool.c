#import <pthread.h>
#include <stdio.h>
#import "../inc/threadpool.h"


void *thread_function(void* arg)
{
    threadpool_t* the_pool = (threadpool_t*)arg;
    do
    {
        pthread_cond_wait(&the_pool->notify, &the_pool->lock);
        task_t my_task = the_pool->task_queue[the_pool->queue_front];
        the_pool->queue_front = (the_pool->queue_front + 1) % QUEUE_SIZE;
        the_pool->queued--;
        pthread_mutex_unlock(&the_pool->lock);
        if (my_task.fn != NULL)
        {
            my_task.fn(my_task.arg);
        }

    } while (the_pool->stop == 0);
    return NULL;
}


void threadpool_init(threadpool_t* pool)
{
    pool->queued = 0;
    pool->queue_front = 0;
    pool->queue_back = 0;
    pool->stop = 0;

    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->notify), NULL);

    for (int i = 0; i < THREADS; i++)
    {
        pthread_create(&(pool->threads[i]), NULL, thread_function, pool);
    }
}

void threadpool_destroy(threadpool_t* pool)
{
    pool->stop = 1;
    pthread_cond_broadcast(&(pool->notify));
    for (int i = 0; i < THREADS; i++)
    {
        pthread_join(pool->threads[i], NULL);
    }
    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));
}

void threadpool_add_task(threadpool_t* pool, void (*function)(void*), void* arg)
{
  task_t task;
  task.fn = function;
  task.arg = arg;

  pthread_mutex_lock(&(pool->lock));
  pool->task_queue[pool->queue_back] = task;
  pool->queue_back = (pool->queue_back + 1) % QUEUE_SIZE;
  pool->queued++;
  pthread_mutex_unlock(&(pool->lock));
  pthread_cond_signal(&(pool->notify));
}

void example_task(void* arg)
{

}
