#import <pthread.h>
#import "../inc/threadpool.h"


void *thread_function(void* arg)
{
    threadpool_t* the_pool = (threadpool_t*)arg;
    do
    {
        pthread_cond_wait(&the_pool->notify, &the_pool->lock);
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

}

void threadpool_add_task(threadpool_t* pool, void (*function)(void*), void* arg)
{

}

void example_task(void* arg)
{

}
