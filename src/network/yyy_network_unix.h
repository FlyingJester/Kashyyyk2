#pragma once

#ifndef YYY_NETWORK_UNIX
#error This header is only meant for use on POSIX using BSD sockets.
#endif

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> /* For gethostbyname */
#include <errno.h>

struct YYY_NetworkSocket{
    struct hostent *host;
    struct sockaddr_in sockaddr;
    int socket;
};

#define yyy_init_sockets()
#define YYY_SOCKET_IS_VALID(SOCKET) (SOCKET->socket >= 0)
#define YYY_CLOSE_SOCKET(SOCKET) (close(SOCKET->socket))
#define YYY_GET_ERROR() errno
#define YYY_EQUALS_ERR(ERR, WHAT) (ERR == WHAT)
#define YYY_BLOCKING_OP() 1

#ifdef __cplusplus
extern "C" {
#endif

void yyy_make_socket_nonblocking(struct YYY_NetworkSocket *socket);

void yyy_make_socket_blocking(struct YYY_NetworkSocket *socket);

#ifdef __cplusplus
}
#endif
