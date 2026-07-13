#import <pthread.h>
#include <stdio.h>
#import <unistd.h>
#import "../inc/threadpool.h"

#include <stdlib.h>


void* thread_function(void* threadpool) {
    threadpool_t* pool = (threadpool_t*)threadpool;
    if (pool == NULL)
    {
        fprintf(stderr, "Invalid threadpool\n");
        return NULL;
    }

    while (1) {
        pthread_mutex_lock(&(pool->lock));

        while (pool->queued == 0 && !pool->stop) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if (pool->stop) {
            pthread_mutex_unlock(&(pool->lock));
            pthread_exit(NULL);
        }

        task_t task = pool->task_queue[pool->queue_front];
        pool->queue_front = (pool->queue_front + 1) % QUEUE_SIZE;
        pool->queued--;

        pthread_mutex_unlock(&(pool->lock));
        if (task.fn != NULL)
        {
            (*(task.fn))(task.arg);
        }

    }

    return NULL;
}


void threadpool_init(threadpool_t* pool)
{
    if (pool == NULL)
    {
        fprintf(stderr, "Invalid threadpool\n");
        return;
    }
    pool->queued = 0;
    pool->queue_front = 0;
    pool->queue_back = 0;
    pool->stop = 0;

    if (pthread_mutex_init(&(pool->lock), NULL) != 0)
    {
        fprintf(stderr, "pthread_mutex_init failed\n");
        return;
    }

    if (pthread_cond_init(&(pool->notify), NULL) != 0)
    {
        fprintf(stderr, "pthread_cond_init failed\n");
        pthread_mutex_destroy(&(pool->lock));
        return;
    }

    for (int i = 0; i < THREADS; i++)
    {
        if (pthread_create(&(pool->threads[i]), NULL, thread_function, pool) != 0)
        {
            fprintf(stderr, "pthread_create failed\n");
        }
    }
    pool->initialized = 1;
}

void threadpool_destroy(threadpool_t* pool)
{
    if (pool == NULL || pool->initialized != 1)
    {
        fprintf(stderr, "Invalid threadpool\n");
        return;
    }
    pthread_mutex_lock(&(pool->lock));
    pool->stop = 1;
    pthread_cond_broadcast(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));
    for (int i = 0; i < THREADS; i++)
    {

        pthread_join(pool->threads[i], NULL);
    }
    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));
}

void threadpool_add_task(threadpool_t* pool, void (*function)(void*), void* arg)
{

    if (pool == NULL || function == NULL || pool->initialized != 1)
    {
        fprintf(stderr, "Invalid task, pool or function is null\n");
        return;
    }


    printf("Adding task to pool\n");

  pthread_mutex_lock(&(pool->lock));

  if (pool->queued < QUEUE_SIZE)
  {
      int next_bck = (pool->queue_back + 1) % QUEUE_SIZE;
      pool->task_queue[pool->queue_back].fn = function;
      pool->task_queue[pool->queue_back].arg = arg;
      pool->queue_back = next_bck;
      pool->queued++;
      pthread_cond_signal(&(pool->notify));
  } else
  {
      printf("Queue is full, task not added\n");
  }

  pthread_mutex_unlock(&(pool->lock));

}

void example_task(void* arg)
{

    if (arg != NULL)
    {
        int* num = (int*)arg;
        printf("Processing task %d\n", *num);
    } else
    {
        fprintf(stderr, "Invalid task argument\n");
        return;
    }

    sleep(1);
    free(arg);// Simulate task work
}
