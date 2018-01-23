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

static int serverListen();
void handleClient(int servfd);

int main()
{
    int servfd = serverListen();
    handleClient(servfd);
    return 0;
}

static int serverListen()
{
    int listenfd;
    struct sockaddr_in servaddr;
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(5556);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    Listen(listenfd, SOMAXCONN);
    return listenfd;
}

void handleClient(int servfd)
{
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    int nready,i,connfd,sockfd;
    fd_set  rset, allset;

    int maxfd = servfd;
    int maxi = -1;
    int client[FD_SETSIZE];
    
    for (i = 0; i < FD_SETSIZE; i++)
    {
        client[i] = -1;
    }
    FD_ZERO(&allset);
    FD_SET(servfd, &allset);

    while (1)
    {
        
        rset = allset;
        //阻塞，若监听套接字或链接套接字可读返回
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        if (nready < 0)
        {
            if (errno == EINTR)
                continue;
            
            ERR_EXIT("select");

        }
        if (nready == 0)
            continue;
        //监听套接字可读
        if (FD_ISSET(servfd, &rset))
        {
            clilen = sizeof(cliaddr);
            connfd = Accept(servfd, (struct sockaddr *) &cliaddr, &clilen);
            printf("ip= %s  port= %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port)); 
            for (i = 0; i < FD_SETSIZE; i++)
            {
                if (client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            }
            
            if (i == FD_SETSIZE)
            {
                ERR_EXIT("too many clients\n");
            }
                

            FD_SET(connfd, &allset);
            
            //更新maxfd
            if (maxfd < connfd)
                maxfd = connfd;
            //更新maxi
            if (i > maxi)
                maxi = i;
             
            if (--nready <= 0)
                continue;  
        }
        //遍历已连接客户数组
        for (i = 0; i <= maxi; i++)
        {
            
            if ( (sockfd = client[i]) < 0)
                continue;
            //若已连接套接字可读
            if (FD_ISSET(sockfd, &rset))
            {
                
                char recvline[1024];
                memset(recvline, 0, sizeof(recvline));
                int ret = readline(sockfd, recvline, sizeof(recvline));
                if (ret < 0)
                    ERR_EXIT("readline\n");
                else if(ret == 0)
                {
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    printf("client close\n");
                    client[i] = -1;
                    exit(0);
                }
                fputs(recvline, stdout);
                writen(sockfd, recvline, strlen(recvline));
                if (--nready <= 0)
                    break; 
            }  
        
        }    
    }
}