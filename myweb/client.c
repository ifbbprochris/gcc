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

int main(void) {
    struct sockaddr_in servaddr;
    int sockfd;
    int n;
    char buf[MAX_LINE];

    sockfd=socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    while (n=read(0, buf, MAX_LINE))
    {
        n=sendto(sockfd, buf, n, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        n=recvfrom(sockfd, buf, MAX_LINE, 0, NULL, 0);
        write(1, buf, n);
    }

    close(sockfd);

    return 0;
    
}