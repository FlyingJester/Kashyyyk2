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

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_InitSocket(struct YYY_NetworkSocket *socket);

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_DestroySocket(struct YYY_NetworkSocket *socket);

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_ConnectSocket(struct YYY_NetworkSocket *socket,
    const char *address, unsigned long port, long timeout_in_microsecond);

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_CloseSocket(struct YYY_NetworkSocket *socket);

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_ReadSocket(
    struct YYY_NetworkSocket * YYY_NETWORK_RESTRICT socket,
    void * YYY_NETWORK_RESTRICT output,
    unsigned long length_to_read,
    unsigned long * YYY_NETWORK_RESTRICT out_length_read);

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_WriteSocket(
    struct YYY_NetworkSocket * YYY_NETWORK_RESTRICT socket,
    const void * YYY_NETWORK_RESTRICT input, unsigned long length_to_write);

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_MakeSocketBlocking(
    struct YYY_NetworkSocket *socket);

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_MakeSocketNonBlocking(
    struct YYY_NetworkSocket *socket);

/*---------------------------------------------------------------------------*/

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
