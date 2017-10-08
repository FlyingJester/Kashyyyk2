/* 
 *  Copyright (c) 2017 Martin McDonough.  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * - Products derived from this software may not be called "Kashyyyk", nor may
 *     "YYY" appear in their name, without prior written permission of
 *     the copyright holders.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/*---------------------------------------------------------------------------*/

#ifndef YYY_NETWORK_WIN32_H
#define YYY_NETWORK_WIN32_H
#pragma once

#ifndef YYY_NETWORK_WIN32
#error This header is only meant for use on Windows, using native WinSock2.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#include <WinSock2.h>

/*---------------------------------------------------------------------------*/

#ifdef __WATCOMC__
typedef int socklen_t;
#else
typedef unsigned long socklen_t;
#endif

/*---------------------------------------------------------------------------*/

struct YYY_NetworkSocket{
    struct hostent *host;
    struct sockaddr_in sockaddr;
    SOCKET socket;
#ifndef YYY_NETWORK_DISABLE_TLS
    unsigned char has_tls;
#endif
};

/*---------------------------------------------------------------------------*/

#define YYY_SOCKET_IS_VALID(SOCKET) (SOCKET->socket != INVALID_SOCKET)

/*---------------------------------------------------------------------------*/

#define YYY_BLOCKING_OP() (WSAGetLastError() == WSAEWOULDBLOCK)

/*---------------------------------------------------------------------------*/

#define YYY_CLOSE_SOCKET(SOCKET)\
    do{ shutdown(SOCKET->socket, SD_SEND); closesocket(SOCKET->socket); }while(0)

/*---------------------------------------------------------------------------*/

#define YYY_GET_ERROR WSAGetLastError

/*---------------------------------------------------------------------------*/

#define YYY_EQUALS_ERR(ERR, WHAT) (ERR == WSA ## WHAT)

/*---------------------------------------------------------------------------*/

void yyy_init_sockets();

/*---------------------------------------------------------------------------*/

static void yyy_make_socket_blocking(struct YYY_NetworkSocket *socket){
    unsigned long m = 0;
    ioctlsocket(socket->socket, FIONBIO, &m);
}

/*---------------------------------------------------------------------------*/

static void yyy_make_socket_nonblocking(struct YYY_NetworkSocket *socket){
    unsigned long m = 1;
    ioctlsocket(socket->socket, FIONBIO, &m);
}

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* YYY_NETWORK_WIN32_H */
