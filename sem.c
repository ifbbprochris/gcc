#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

#define NUM 5

sem_t blanknum, goodsnum;

int q[NUM];

void *producer(void *arg) {
    int i = 0;
    while (1)
    {
        sem_wait(&blanknum);
        q[i]=rand()%100+1;
        printf("producer %d\n", q[i]);
        sem_post(&goodsnum);
        i=(i+1)%NUM;
        
        sleep(rand()%3);
    }
    
}

void *cosumner(void *arg) {
    int i = 0;
    while (1)
    {
        sem_wait(&goodsnum);
        printf("cosumner %d\n", q[i]);
        q[i]=0;
        sem_post(&blanknum);
        i=(i+1)%NUM;
        
        sleep(rand()%3);
    }
    
}


int main() {

    srand(time(NULL));

    sem_init(&blanknum, 0 , NUM);
    sem_init(&goodsnum, 0 , 0);

    pthread_t pid, tid;
    pthread_create(&pid, NULL, producer, NULL);
    pthread_create(&tid, NULL, cosumner, NULL);

    pthread_join(pid, NULL);
    pthread_join(tid, NULL);
    
    return 0;
}