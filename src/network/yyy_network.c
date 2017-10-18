/* 
 *  Copyright (c) 2016-2017 Martin McDonough.  All rights reserved.
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

#include "yyy_network.h"

#ifdef YYY_NETWORK_WIN32
#include "yyy_network_win32.h"
#elif defined YYY_NETWORK_UNIX
#include "yyy_network_unix.h"
#endif

#ifndef YYY_NETWORK_DISABLE_TLS
#define YYY_TLS_INTERNAL 1
#include "yyy_network_tls.h"
#undef YYY_TLS_INTERNAL
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

unsigned YYY_NetworkSocketSize(){
    return sizeof(struct YYY_NetworkSocket);
}

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_InitSocket(struct YYY_NetworkSocket *socket){
    assert(socket != NULL);
    yyy_init_sockets();
    if(socket == NULL)
        return eYYYNetworkFailure;
    memset(socket, 0, sizeof(struct YYY_NetworkSocket));
    return eYYYNetworkSuccess;
}

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_DestroySocket(struct YYY_NetworkSocket *socket) {
    (void)socket;
    assert(socket != NULL);
    return (socket == NULL) ? eYYYNetworkFailure : eYYYNetworkSuccess;
}

/*---------------------------------------------------------------------------*/

/* We have no backend, we must dummy this symbol out. */
#ifdef YYY_NETWORK_DISABLE_TLS
enum YYY_NetworkError YYY_EnableSocketTLS(struct YYY_NetworkSocket *s){
    return eYYYNetworkFailure;
}
#endif

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_ConnectSocket(struct YYY_NetworkSocket *a_socket,
    const char *address, unsigned long port, long timeout_in_microsecond){
    int err = 0;
    
    assert(a_socket != NULL);
    assert(address != NULL);
    
    /* Create the socket to connect to the specified host/port */
    if(!(a_socket->host = gethostbyname(address)))
        return eYYYNetworkAddressUnreachable;
    
    a_socket->sockaddr.sin_family = AF_INET;
    a_socket->sockaddr.sin_port = htons((unsigned short)port);
    
    {
        /* Try to construct the address */
        struct in_addr *const in_address =
            (void*)(a_socket->host->h_addr_list[0]);
        if(in_address == NULL)
            return eYYYNetworkAddressUnreachable;
        a_socket->sockaddr.sin_addr.s_addr = inet_addr(inet_ntoa(*in_address));
    }
    
    /* Create the socket */
    a_socket->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if(!YYY_NETWORK_LIKELY(YYY_SOCKET_IS_VALID(a_socket)))
        return eYYYNetworkFatal;
    
    /* We make the socket nonblocking first, so that if there is a timeout we
     * can use select to wait for the socket to become ready.
     */
    if(timeout_in_microsecond < 0)
        YYY_MakeSocketNonBlocking(a_socket);
    
    err = connect(a_socket->socket, (const void*)(&a_socket->sockaddr),
        sizeof(struct sockaddr_in));
    
    /* Check for error */
    if(err < 0){
        err = YYY_GET_ERROR();
        YYY_CLOSE_SOCKET(a_socket);
        if(YYY_EQUALS_ERR(err, ECONNREFUSED))
            return eYYYNetworkRefused;
        else if(YYY_EQUALS_ERR(err, EISCONN))
            return eYYYNetworkAlreadyConnected;
        else if(YYY_EQUALS_ERR(err, ETIMEDOUT))
            return eYYYNetworkTimeout;
        else{
            return eYYYNetworkFailure;
        }
    }
    
    if(timeout_in_microsecond >= 0l)
        YYY_MakeSocketNonBlocking(a_socket);
    
    /* If there is a timeout, and the connect operation is not done, select the
     * socket to wait until some event has occured on it. 
     */
    if(timeout_in_microsecond >= 0 && YYY_BLOCKING_OP()){
        struct timeval time;
        fd_set set;
        
        time.tv_sec=timeout_in_microsecond/1000000;
        time.tv_usec=(timeout_in_microsecond%1000000);
        
        FD_ZERO(&set);
        FD_SET(a_socket->socket, &set);
        
        err = select(a_socket->socket+1, &set, NULL, NULL, &time);
        
        if(err==0){
            YYY_CLOSE_SOCKET(a_socket);
            return eYYYNetworkTimeout;
        }
    }
    
    /* Check for error */
    if(err < 0){
        err = YYY_GET_ERROR();
        YYY_CLOSE_SOCKET(a_socket);
        if(YYY_EQUALS_ERR(err, ECONNREFUSED))
            return eYYYNetworkRefused;
        else if(YYY_EQUALS_ERR(err, EISCONN))
            return eYYYNetworkAlreadyConnected;
        else if(YYY_EQUALS_ERR(err, ETIMEDOUT))
            return eYYYNetworkTimeout;
        else
            return eYYYNetworkFailure;
    }
    
    /* Restore the socket to blocking again */
    YYY_MakeSocketBlocking(a_socket);
    
#ifndef YYY_NETWORK_DISABLE_TLS
    if(a_socket->has_tls)
        YYY_TLSBeginConnection(a_socket);
#endif

    return eYYYNetworkSuccess;
}

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_CloseSocket(struct YYY_NetworkSocket *a_socket){
    if(a_socket == NULL)
        return eYYYNetworkFailure;
    if(!YYY_SOCKET_IS_VALID(a_socket))
        return eYYYNetworkNotConnected;
#ifndef YYY_NETWORK_DISABLE_TLS
    if(a_socket->has_tls)
        YYY_TLSEndConnection(a_socket);
#endif
    YYY_CLOSE_SOCKET(a_socket);
    return eYYYNetworkSuccess;
}

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_ReadSocket(
    struct YYY_NetworkSocket * YYY_NETWORK_RESTRICT a_socket,
    void * YYY_NETWORK_RESTRICT output,
    unsigned long length_to_read,
    unsigned long * YYY_NETWORK_RESTRICT out_length_read){
    
    assert(a_socket != NULL);
    assert(out_length_read != NULL || length_to_read == 0);
    assert(output != NULL || length_to_read == 0);
    
    if(length_to_read == 0)
        out_length_read[0] = 0;
    else{
        const int err = recv(a_socket->socket, output, length_to_read, 0);

        if(err < 0){
            out_length_read[0] = 0;
            return eYYYNetworkFailure;
        }
        else{
#ifndef YYY_NETWORK_DISABLE_TLS
            size_t n;
            void *const decoded =
                YYY_TLSDecode(a_socket, output, length_to_read, &n);
            assert(n <= length_to_read);
            if(decoded != output)
                memcpy(output, decoded, n);
            YYY_TLSFreeDecode(a_socket, decoded);
            out_length_read[0] = n;
#else
            out_length_read[0] = err;
#endif
        }
    }
    /* TODO: Check for closure error. */
    
    return eYYYNetworkSuccess;
}

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_WriteSocket(
    struct YYY_NetworkSocket * YYY_NETWORK_RESTRICT socket,
    const void * YYY_NETWORK_RESTRICT input, unsigned long length_to_write){
    
    assert(socket != NULL);
    assert(input != NULL || length_to_write == 0);
        
    if(length_to_write > 0){
        const void *to_send = input;
        size_t write_len = length_to_write;
#ifndef YYY_NETWORK_DISABLE_TLS
        to_send = YYY_TLSEncode(socket, input, length_to_write, &write_len);
#endif
        const long err = send(socket->socket, to_send, write_len, 0);
#ifndef YYY_NETWORK_DISABLE_TLS
        YYY_TLSFreeEncode(socket, (void*)to_send);
#endif
        if(err < 0)
            return eYYYNetworkFailure;
        /* TODO: Handle specific errors. */
    }
    
    return eYYYNetworkSuccess;
}

enum YYY_NetworkError YYY_MakeSocketBlocking(
    struct YYY_NetworkSocket *socket){
    if(socket == NULL)
        return eYYYNetworkFailure;
    yyy_make_socket_blocking(socket);
    return eYYYNetworkSuccess;
}

enum YYY_NetworkError YYY_MakeSocketNonBlocking(
    struct YYY_NetworkSocket *socket){
    if(socket == NULL)
        return eYYYNetworkFailure;
    yyy_make_socket_nonblocking(socket);
    return eYYYNetworkSuccess;    
}

enum YYY_NetworkError YYY_SocketStatus(
    const struct YYY_NetworkSocket *socket){
    
    int status = 0;
    socklen_t len = sizeof(int);
    const int err = getsockopt(socket->socket, SOL_SOCKET, SO_ERROR, (char*)(&status), &len);
    
    return (err == 0) ? eYYYNetworkSuccess : eYYYNetworkDisconnected;
}

