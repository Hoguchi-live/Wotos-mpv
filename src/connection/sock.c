#include "sock.h"

#define QUEUE_SIZE 100

inline bool _SetSocketBlockingEnabled(int fd, bool blocking);

struct Socket *socket_create(){
    struct Socket *self = malloc(sizeof(struct Socket));
    self->queue = queue_create();
    return self;
}

void socket_init(struct Socket* self){
    if ((self->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Create socket address
    // Forcefully attach socket to the port
    self->opt = 1;
    if (setsockopt(self->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(self->opt), sizeof(int))){
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
    self->address.sin_family = AF_INET;
    self->address.sin_addr.s_addr = INADDR_ANY;
    self->address.sin_port = htons(SERVER_PORT);

    // Bind socket to address
    if (bind(self->server_fd, (struct sockaddr *)&(self->address), sizeof(self->address)) < 0){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    queue_init(self->queue, QUEUE_SIZE);
}

struct Socket *socket_init_start(){
    struct Socket *sock = socket_create();
    socket_init(sock);
    socket_listen(sock);
    socket_accept(sock);
    return sock;
}

void socket_destroy(struct Socket* self){
    // Close the endpoint and listening socket
    close(self->endpoint);
    shutdown(self->server_fd, SHUT_RDWR);

    free(self->queue);
    free(self);
}

void socket_listen(struct Socket *self){
    // listen to incoming connections
    if (listen(self->server_fd, QUEUE_LENGTH) < 0){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

void socket_accept(struct Socket *self){ 
    int addrlen = sizeof(self->address);
    if ((self->endpoint = accept(self->server_fd, (struct sockaddr *)&(self->address), (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    if(_SetSocketBlockingEnabled(self->endpoint, 0)){
        perror("Setting selfet as blocking failed");
        exit(EXIT_FAILURE);
    }
}

void update_queue(struct Socket* self){
    
}

/** 
  Sets socket as blocking
  Returns true on success, or false if there was an error 
*/
bool _SetSocketBlockingEnabled(int fd, bool blocking)
{
   if (fd < 0) return false;

#ifdef _WIN32
   unsigned long mode = blocking ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags == -1) return false;
   flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}
