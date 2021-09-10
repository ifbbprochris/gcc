#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

pthread_t ntid;
void printid(char *);
void *thr_fn(void *arg) {
    printid(arg);
    printf("%s ntid=%p\n", arg, ntid);
    return NULL;
}

void printid(char *tip) {
    pid_t pid=getpid();
    pthread_t tid=pthread_self();
    printf("%s pid:%u tid:%u (%p)\n", tip, pid, tid, tid);
    return;
}

int main(){
    int ret;
    ret=pthread_create(&ntid, NULL, thr_fn, "new thread");
    if(ret) {
	printf("create thread error:%s\n", strerror(ret));
    }
    
    pthread_join(ntid, NULL);
    printid("main thread");
    return 0;
}
