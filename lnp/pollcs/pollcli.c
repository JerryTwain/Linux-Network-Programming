#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "socketio.h"
#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(5556);
    Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    
    char sendline[1024], recvline[1024];
    memset(sendline, 0, sizeof(sendline));
    memset(recvline, 0, sizeof(recvline));

    while (fgets(sendline, sizeof(sendline), stdin) != NULL)
    {
        ssize_t ret;
        writen(sockfd, sendline, strlen(sendline));
        ret = readline(sockfd, recvline, sizeof(recvline));
        if (ret < 0)
            ERR_EXIT("readline");
        else if (ret == 0)
        {
            printf("server close\n");
            break;
        }
        fputs(recvline, stdout);
        memset(sendline, 0, sizeof(sendline));
        memset(recvline, 0, sizeof(recvline));
        
    }
    close(sockfd);
    return 0;

}