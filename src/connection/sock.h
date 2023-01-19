#ifndef _SOCK_H_
#define _SOCK_H_

#include <stdlib.h> 
#include <stdio.h> 

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#define QUEUE_LENGTH 10
#define RECV_BUFFER_SIZE 2048
#define SERVER_PORT 10000

struct Socket{
   int server_fd;   // File descriptor
   struct sockaddr_in address; // Socket address
   int opt;
   int endpoint; 
};

struct Socket *socket_create();
void socket_init(struct Socket* socket);
void socket_listen(struct Socket* socket);

#endif
