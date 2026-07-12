#import <pthread.h>
#import "../inc/threadpool.h"


void *thread_function(void* arg)
{
    threadpool_t* the_pool = (threadpool_t*)arg;
    while (1)
    {
        pthread_cond_wait(&the_pool->notify, &the_pool->lock);
        if (the_pool->stop)
            break;
        task_t my_task = the_pool->task_queue[the_pool->queue_front];
        the_pool->queue_front = (the_pool->queue_front + 1) % QUEUE_SIZE;
        the_pool->queued--;
        pthread_mutex_unlock(&the_pool->lock);
        my_task.fn(my_task.arg);
    }
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
