#import <pthread.h>
#import <stdlib.h>
#import <stdio.h>
#import <unistd.h>
#import "../inc/threadpool.h"


int main(void)
{
     threadpool_t pool;
     threadpool_init(&pool);

     for (int i = 0; i < 15; i++)
     {
          int* idx = malloc(sizeof(int));
          if (idx == NULL)
          {
               printf("Failed to allocate memory for task index\n");
               return 1;
          }
          *idx = i;
          threadpool_add_task(&pool, example_task, idx);
     }

     sleep(4);

     threadpool_destroy(&pool);
     return 0;
}



