#include <stdio.h>
#include <pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

typedef struct Goods {
    int data;
    struct Good *next;
} Goods;

Goods *head = NULL;

pthread_mutex_t headlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t hasgood = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    Goods *ng;
    while (1)
    {
        ng=(Goods *)malloc(sizeof(Goods));
        ng->data=rand()%100;
        pthread_mutex_lock(&headlock);
        ng->next = head;
        head = ng;
        pthread_mutex_unlock(&headlock);
        pthread_cond_signal(&hasgood);

        printf("producer %d\n", ng->data);
        sleep(rand()%3);
    }
    return 0;
}

void *consumer(void *arg) {
    Goods *k;
    while (1)
    {
        pthread_mutex_lock(&headlock);
        if(!head) {
            pthread_cond_wait(&hasgood, &headlock);
        }
        k=head;
        head=head->next;
        pthread_mutex_unlock(&headlock);
        printf("consumer %d\n", k->data);
        free(k);
        sleep(rand()%3);
    }
    return 0;
}

int main() {
    pthread_t ptid, ctid;

    pthread_create(&ptid, NULL, producer, NULL);
    pthread_create(&ctid, NULL, consumer, NULL);

    pthread_join(ptid, NULL);
    pthread_join(ctid, NULL);

    return 0;
}

