#include <pthread.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include  <math.h>
#define NUM_THREADS 4  
  
void *BusyWork(void *t) {  
    double result=0.0;  
    long tid = (long)t;  
    printf("Thread %ld starting...\n",tid);  
    for (int i=0; i<1000000; i++)  
    {  
        result = result + sin(i) * tan(i);  
    }  
    printf("Thread %ld done. Result = %e\n",tid, result);  
    pthread_exit((void*) t);  
}  
  
int main (int argc, char *argv[]) {  
    pthread_t thread[NUM_THREADS];  
    pthread_attr_t attr;  
  
    // 1/4: init  
    pthread_attr_init(&attr);  
    // 2/4: explicitly specify as joinable or detached  
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);  
  
    int rc;  
    long t;  
    for(t=0; t<NUM_THREADS; t++)  
    {  
        printf("Main: creating thread %ld\n", t);  
        // 3/4: use thread attribute  
        rc = pthread_create(&thread[t], &attr, BusyWork, (void *)t);  
        if (rc) {  
            printf("ERROR; return code from pthread_create() is %d\n", rc);  
            exit(-1);  
        }  
    }  
  
    // 4/4: release thread attribute  
    pthread_attr_destroy(&attr);  
  
    void *status;  
    for(t=0; t<NUM_THREADS; t++)  
    {  
        rc = pthread_join(thread[t], &status);  
        if (rc) {  
            printf("ERROR; return code from pthread_join() is %d\n", rc);  
            exit(-1);  
        }  
        printf("Main: completed join with thread %ld having a status of %ld\n",t,(long)status);  
    }  
  
    printf("Main: program completed. Exiting.\n");  
    return 0;  
}  