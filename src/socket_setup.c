#include "socket_setup.h"

inline bool _SetSocketBlockingEnabled(int fd, bool blocking);

void socket_accept(){ 
    int addrlen = sizeof(sock->address);
    if ((sock->endpoint = accept(sock->server_fd, (struct sockaddr *)&(sock->address), (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    if(_SetSocketBlockingEnabled(sock->endpoint, 0)){
        perror("Setting socket as blocking failed");
        exit(EXIT_FAILURE);
    }
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
