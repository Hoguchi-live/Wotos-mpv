#ifndef _SOCK_H_
#define _SOCK_H_

#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> //TODO Unnecessary
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>

#include "../util/safe_queue.h"

#define QUEUE_LENGTH 10
#define RECV_BUFFER_SIZE 2048
#define SERVER_PORT 10000

struct Socket{
   int server_fd;   // File descriptor
   struct sockaddr_in address; // Socket address
   int opt;
   int endpoint; 
   queue_t *queue;
};

struct Socket *socket_create();
void socket_init(struct Socket*);
void socket_destroy(struct Socket*);
struct Socket *socket_init_start();

void socket_listen(struct Socket*);
void socket_accept();

void update_queue(struct Socket*);


#endif
