#include "socketio.h"

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)


int Socket(int family, int type, int protocol)
{
    int n;
    n = socket(family, type, protocol);
    if (n < 0)
        ERR_EXIT("socket");
    return n;
}

int Connect(int sockfd, struct sockaddr * serv_addr, int addrlen)
{
    int n;
    n = connect(sockfd, serv_addr, addrlen);
    if (n < 0)
        ERR_EXIT("connect");
    return n;
}

ssize_t readn(int fd, void *vptr, size_t n)
{
    ssize_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ( (nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;
            else
                return (-1);
        }
        else if (nread == 0)
            break;
        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft);
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return(-1);
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

ssize_t recv_peek(int sockfd, void *vptr, size_t n)
{
    while (1)
    {
        int ret = recv(sockfd, vptr, n, MSG_PEEK);
        if (ret < 0 && errno == EINTR)
            continue;
        return ret;
    }
}

ssize_t readline(int sockfd, void *vptr, size_t maxline)
{
    ssize_t ret;
    int nread;
    char *ptr = vptr;
    int nleft = maxline;
    //先把全部数据读入缓冲区
    while (1)
    {
        ret = recv_peek(sockfd, ptr, nleft);
        if (ret < 0)
            return ret;
        else if (ret == 0)
            return ret;
        //在看缓冲区窥视是否有\n
        nread = ret;
        int i;
        for (i = 0; i < nread; i++)
        {
            if (ptr[i] == '\n')
            {
                //把包括\n和之前的数据读入缓冲区
                ret = readn(sockfd, ptr, i+1);
                if (ret != i+1)
                    exit(EXIT_FAILURE);
                return ret;
            }
        }
        
        if (nread > nleft)
            exit(EXIT_FAILURE);
        nleft -= nread;
        ret = readn(sockfd, ptr, nread);
        if (ret != nread)
            exit(EXIT_FAILURE);

        ptr += nread;
    }
    return -1;
}

int Accept(int sock, struct sockaddr* cliaddr, socklen_t *addrlen)
{
    int n;
    n = accept(sock, cliaddr, addrlen);
    if (n < 0)
        ERR_EXIT("accept");
    return n;
}

int Listen(int sockfd, int backlog)
{
    int n;
    n = listen(sockfd, backlog);
    if (n < 0)
        ERR_EXIT("listen");
    return n;
}

int Bind(int listenfd, struct sockaddr* servaddr, socklen_t addrlen)
{
    int n;
    n = bind(listenfd, servaddr, addrlen);
    if (n < 0)
        ERR_EXIT("bind");
    return n;
}