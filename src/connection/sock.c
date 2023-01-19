#include "sock.h"

struct Socket *socket_create(){
    struct Socket *self = malloc(sizeof(struct Socket));
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
}

void socket_listen(struct Socket *self){
    // listen to incoming connections
    if (listen(self->server_fd, QUEUE_LENGTH) < 0){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}
