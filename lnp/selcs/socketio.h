#ifndef SOCKETIO_H
#define SOCKETIO_H
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int Socket(int family, int type, int protocol);
int Connect(int sockfd, struct sockaddr * serv_addr, int addrlen);
ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t recv_peek(int sockfd, void *vptr, size_t n);
ssize_t readline(int sockfd, void *vptr, size_t maxline);
int Accept(int sock, struct sockaddr* cliaddr, socklen_t *addrlen);
int Listen(int sockfd, int backlog);
int Bind(int listenfd, struct sockaddr* servaddr, socklen_t addrlen);


#endif