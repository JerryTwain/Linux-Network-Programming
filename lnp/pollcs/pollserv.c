#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <poll.h>
#include "socketio.h"

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

#define OPEN_MAX 2048

int main()
{
    int i,listenfd, connfd, maxi, sockfd;
    int nready;
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(5556);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    Listen(listenfd, SOMAXCONN);

    //初始化
    maxi = 0;
    for (i = 0; i < OPEN_MAX; i++)
    {
        client[i].fd = -1;
    }
    client[0].fd = listenfd;
    client[0].events = POLLIN;

    while (1)
    {    
        //阻塞，若监听套接字或链接套接字可读返回
        nready = poll(client, maxi+1, -1);
        //监听套接字可读
        if (nready < 0)
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("poll");
        }
        if (nready == 0)
            continue;
        //判断是否有读事件发生
        if (client[0].revents & POLLIN)
        {
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
            //寻找空闲的端口给予连接套接字使用
            for (i = 0; i < OPEN_MAX; i++)
            {
                if (client[i].fd < 0)
                {
                    client[i].fd = connfd;
                    break;
                }
            }
            //端口超出最大范围
            if (i == OPEN_MAX)
            {
                ERR_EXIT("too many clients\n");
            }
            printf("ip= %s  port= %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port)); 
            client[i].events = POLLIN;
            
            //更新maxi
            if (i > maxi)
                maxi = i;
             
            if (--nready <= 0)
                continue;  
        }
        //遍历已连接客户数组
        for (i = 1; i <= maxi; i++)
        {         
            if ( (sockfd = client[i].fd) < 0)
                continue;
            //若已连接套接字可读
            if (client[i].events & POLLIN)
            {     
                char recvline[1024];
                memset(recvline, 0, sizeof(recvline));
                int ret = readline(sockfd, recvline, sizeof(recvline));
                if (ret < 0)
                    ERR_EXIT("readline\n");
                else if(ret == 0)
                {
                    close(sockfd);
                    printf("client close\n");
                    client[i].fd = -1;
                    exit(0);
                }
                fputs(recvline, stdout);
                writen(sockfd, recvline, strlen(recvline));
                if (--nready <= 0)
                    break;   
            }
        }

    }
 
    return 0;
    
}