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

#ifndef YYY_NETWORK_H
#define YYY_NETWORK_H
#pragma once

/*---------------------------------------------------------------------------*/
/* Use this temporary define to allow opening network_common.h */
#define YYY_NETWORK_HEADER_1
#include "yyy_network_common.h"
#undef YYY_NETWORK_HEADER_1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup YYY_Socket Basic socket functionality
 * @{
 */

struct YYY_NetworkSocket;

/*---------------------------------------------------------------------------*/

YYY_NETWORK_CONST(
    unsigned YYY_NetworkSocketSize()
);

/**
 * @brief Initializes a socket.
 *
 * The socket must have been allocated by the application. You can get the size
 * of a YYY_NetworkSocket with YYY_NetworkSocketSize. You must use
 * YYY_InitSocket before any other function can be called on the socket.
 */
enum YYY_NetworkError YYY_InitSocket(struct YYY_NetworkSocket *socket);

/**
 * @brief Destroys the resources associated with a socket.
 *
 * @note It is the application's responsibility to allocate and free the memory
 * that @p socket actually points to. This function just frees the resources
 * allocated by YYY_InitSocket.
 */
enum YYY_NetworkError YYY_DestroySocket(struct YYY_NetworkSocket *socket);

/**
 * @brief Enables a TLS/SSL with the socket.
 *
 * This must be called after YYY_InitSocket, but before YYY_ConnectSocket.
 * If yyy_network was compiled with YYY_NETWORK_DISABLE_TLS then this function
 * will always return failure.
 */
enum YYY_NetworkError YYY_EnableSocketTLS(struct YYY_NetworkSocket *socket);

/**
 * @brief Tries to connect a socket to the specified address.
 *
 * The socket must be initialized with YYY_InitSocket. If SSL/TLS is needed,
 * YYY_EnableSocketTLS must have been called on the socket before this
 * function is used.
 *
 * @param socket Socket to connect
 * @param address Address to connect to. This can be an IP address, a hostname,
 *    or a DNS-resolvable name.
 * @param port Port to connect to. Depending on the platform, ports may be
 *    limited to 1-0xFFFF.
 * @param timeout_in_microsecond Timeout for connection. If a negative number
 *    is specified, the timeout defaults to the system default (usually very
 *    long).
 */
enum YYY_NetworkError YYY_ConnectSocket(struct YYY_NetworkSocket *socket,
    const char *address, unsigned long port, long timeout_in_microsecond);

/**
 * @brief Closes connected socket.
 *
 * The socket cannot be reused. The only valid function call on the socket
 * after YYY_CloseSocket is YYY_DestroySocket.
 */
enum YYY_NetworkError YYY_CloseSocket(struct YYY_NetworkSocket *socket);

/**
 * @brief Reads data from a socket.
 *
 * Reads @p length_to_read from @p socket. If the socket has been marked as
 * non-blocking, then any available data up to @p length_to_read is read.
 * Otherwise, this call blocks until @p length_to_read is read or an error
 * occurs. Either way, the amount of data that has been read is placed into
 * @p out_length_read.
 *
 * @param socket Socket to read from. Must be connected.
 * @param output Buffer to place data in.
 * @param length_to_read Maximum amount to read.
 * @param out_length_read [out] Receives the amount that was read.
 */
enum YYY_NetworkError YYY_ReadSocket(
    struct YYY_NetworkSocket * YYY_NETWORK_RESTRICT socket,
    void * YYY_NETWORK_RESTRICT output,
    unsigned long length_to_read,
    unsigned long * YYY_NETWORK_RESTRICT out_length_read);

/**
 * @brief Writes data to a socket.
 */
enum YYY_NetworkError YYY_WriteSocket(
    struct YYY_NetworkSocket * YYY_NETWORK_RESTRICT socket,
    const void * YYY_NETWORK_RESTRICT input, unsigned long length_to_write);

/**
 * @brief Makes reads from a socket blocking.
 *
 * @sa YYY_ReadSocket
 * @sa YYY_MakeSocketNonBlocking
 */
enum YYY_NetworkError YYY_MakeSocketBlocking(
    struct YYY_NetworkSocket *socket);

/**
 * @brief Makes reads from a socket non-blocking.
 *
 * @sa YYY_ReadSocket
 * @sa YYY_MakeSocketBlocking
 */
enum YYY_NetworkError YYY_MakeSocketNonBlocking(
    struct YYY_NetworkSocket *socket);

/**
 * @brief Checks if a socket is connected, disconnected, or has an error.
 */
YYY_NETWORK_WARN_UNUSED_RESULT(
    enum YYY_NetworkError YYY_SocketStatus(
        const struct YYY_NetworkSocket *socket)
);

/**
 * @}
 */

/**
 * @defgroup YYY_Select Select/Poll/KQueue functionality
 * @{
 */

struct YYY_SocketGroup;

/*---------------------------------------------------------------------------*/

YYY_NETWORK_CONST(
    unsigned YYY_SocketGroupSize()
);

/*---------------------------------------------------------------------------*/

/**
 * @brief Returns true if a group must be locked before modification.
 *
 * If this is true, then the client must ensure mutually exclusive access to
 * the functions on a socket group. If false, then the client can call any
 * functions on the group at any time (excluding Init/Destroy) and the calls
 * will not cause inconsistencies. For instance, you can add or remove a socket
 * from the group while another thread is waiting on the group.
 *
 * This will generally only return true for the select() backend.
 *
 * This does not apply to any actions taken on the sockets in the group.
 *
 * @return 0 if locking is not needed, 1 otherwise.
 */
YYY_NETWORK_CONST(
    unsigned YYY_SocketGroupNeedsLocking()
);

/*---------------------------------------------------------------------------*/

void YYY_InitSocketGroup(struct YYY_SocketGroup *group);

/*---------------------------------------------------------------------------*/

void YYY_DestroySocketGroup(struct YYY_SocketGroup *group);

/*---------------------------------------------------------------------------*/

void YYY_AddSocketToGroup(struct YYY_NetworkSocket *socket, void *user_data,
    struct YYY_SocketGroup *group);

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_RemoveSocketFromGroup(struct YYY_NetworkSocket *socket,
    struct YYY_SocketGroup *group, void **out_user_data);

/*---------------------------------------------------------------------------*/

void YYY_PokeSocketGroup(struct YYY_SocketGroup *group);

/*---------------------------------------------------------------------------*/

struct YYY_WaitResult;

/*---------------------------------------------------------------------------*/

void YYY_DestroyWaitResult(struct YYY_WaitResult *res);

/*---------------------------------------------------------------------------*/

/**
 * Returns eYYYNetworkSuccess if there is a new result. Returns
 * eYYYNetworkFailure if there are no further results.
 */
enum YYY_NetworkError YYY_GetNextWaitResult(struct YYY_WaitResult *r,
    void **out_user_data);

/*---------------------------------------------------------------------------*/

/**
 * @brief waits until data can be read, or until an error occurs.
 *
 * Note that disconnects are not discovered immediately, and there is no
 * guarantee that a disconnect will be found by YYY_Network at all. It is
 * expected that the application will implement some form of check that the
 * other end of the socket is still alive. Most application protocols include
 * this as some kind of ping/timeout.
 *
 * @arg group Socket Group to wait for event on
 * @arg timeout_in_microsecond Timeout of wait in microseconds. This will be
 *   rounded up to the nearest system clock cycle. 0 means non-blocking, and
 *   will return immediately if no event is pending.
 * @arg out_result On eSuccess, this will be a YYY_WaitResult which contains
 *   the resulting sockets that have data to read. Set to NULL otherwise.
 * @return eDisconnected on disconnection, eSuccess on data to read, or ePoked
 * if the group has been poked.
 *
 * @sa YYY_GetNextWaitResult
 * @sa YYY_DestroyWaitResult
 * @sa YYY_PokeSocketGroup
 */
YYY_NETWORK_WARN_UNUSED_RESULT(
enum YYY_NetworkError YYY_WaitOnSocketGroup(struct YYY_SocketGroup *group,
    long timeout_in_microsecond,
    struct YYY_WaitResult **out_result)
);

/**
 * @}
 */
 
#ifdef __cplusplus
}
#endif

#endif
