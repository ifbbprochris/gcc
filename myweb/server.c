#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/wait.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERV_PORT 8000
#define MAX_LINE 80

#define prreixt(msg) \
    {                \
        perror(msg); \
        exit(1);     \
    }

typedef struct Task
{
    int fd;
    struct Task *next;
} Task;

typedef struct Task_Pool
{
    Task *head;
    Task *tail;
    pthread_mutex_t lock;
    pthread_cond_t havetask;
} Task_Pool;

Task_Pool *task_pool_init()
{
    Task_Pool *tp = (Task_Pool *)malloc(sizeof(Task_Pool));
    tp->head = NULL;
    tp->tail = NULL;
    pthread_mutex_init(&tp->lock, NULL);
    pthread_cond_init(&tp->havetask, NULL);
    return tp;
}

void task_pool_push(Task_Pool *tp, int fd)
{
    pthread_mutex_lock(&tp->lock);

    Task *t = (Task *)malloc(sizeof(Task));
    t->fd = fd;
    t->next = NULL;
    if (!tp->tail)
    {
        tp->head = tp->tail = t;
    }
    else
    {
        tp->tail->next = t;
        tp->tail = t;
    }
    pthread_cond_broadcast(&tp->havetask);
    pthread_mutex_unlock(&tp->lock);
}

Task task_pool_pop(Task_Pool *tp)
{
    pthread_mutex_lock(&tp->lock);

    while (tp->head == NULL)
    {
        pthread_cond_wait(&tp->havetask, &tp->lock);
    }

    Task tmp, *k;
    k = tp->head;
    tmp = *k;
    tp->head = tp->head->next;

    if (!tp->head)
    {
        tp->tail = NULL;
    }
    free(k);
    pthread_mutex_unlock(&tp->lock);
    return tmp;
}

void task_pool_free(Task_Pool *tp)
{
    pthread_mutex_lock(&tp->lock);

    Task *p = tp->head, *k;
    while (p)
    {
        k = p;
        p = p->next;
        free(k);
    }
    tp->head = NULL;

    pthread_mutex_unlock(&tp->lock);
    pthread_mutexattr_destroy(&tp->lock);
    pthread_cond_destroy(&tp->havetask);
    free(tp);
    return;
}

void *up_server(void *arg)
{
    pthread_detach(pthread_self());

    Task_Pool *tp = arg;
    char buf[MAX_LINE];
    int n, i;

    while (1)
    {
        Task tmp = task_pool_pop(tp);
        int connfd = tmp.fd;
        printf("get task fd=%d\n", connfd);
        while (1)
        {
            n = read(connfd, buf, MAX_LINE);
            if (!strncmp(buf, "quit", 4))
                break;
            write(1, buf, n);
            for (i = 0; i < n; i++)
                buf[i] = toupper(buf[i]);
            write(connfd, buf, n);
        }
        printf("finish task fd=%d\n", connfd);
        close(connfd);
    }

    return (void *)0;
}

int main()
{

    char buf[MAX_LINE];

    int connfd, listenfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int n, i;

    char str[INET_ADDRSTRLEN];

    Task_Pool *tp = task_pool_init();

    pthread_t tid;
    for (i = 0; i < 4; i++)
    {
        pthread_create(&tid, NULL, up_server, (void *)tp);
        printf("new thread is %#lx\n", tid);
    }

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        prreixt("bind");

    if (listen(listenfd, 2) < 0)
        prreixt("listend");

    printf("accepting connections...\n");

    while (1)
    {
        cliaddr_len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
        if (connfd < 0)
            prreixt("accept");

        printf("receive from %s:%d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));

        // 进来玩
        task_pool_push(tp, connfd);
    }

    task_pool_free(tp);
    return 0;
}