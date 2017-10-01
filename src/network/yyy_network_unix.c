#include "yyy_network_unix.h"

#include <fcntl.h>

void yyy_make_socket_nonblocking(struct YYY_NetworkSocket *socket){
    const int status = fcntl(socket->socket, F_GETFL) | O_NONBLOCK;
    fcntl(socket->socket, F_SETFL, status);
}

void yyy_make_socket_blocking(struct YYY_NetworkSocket *socket){
    const int status = fcntl(socket->socket, F_GETFL) & (int)~O_NONBLOCK;
    fcntl(socket->socket, F_SETFL, status);
}
