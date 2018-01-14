/* 
 *  Copyright (c) 2014-2017 Martin McDonough.  All rights reserved.
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

#ifndef KASHYYYK_YYY_MAIN_H
#define KASHYYYK_YYY_MAIN_H
#pragma once

/*---------------------------------------------------------------------------*/

#include "network/yyy_network.h"

#include "utils/yyy_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined _MSC_VER) || (defined __WATCOMC__)
#define YYY_FASTCALL __fastcall
#else
#define YYY_FASTCALL
#endif

/**
 * @brief This is what we consider the "safe" message length to send.
 *
 * Even a server that only implements the original RFC (And does it badly)
 * should be able to accept a message of this size.
 *
 * This is 256 (-1 for stupidity/safety) -2 for crlf -32 for max old-school
 * channel length -sizeof(PRIVMSG) - sizeof( :). Although we aren't worried
 * about crashing or compromising a server, we are worried that a server will
 * just not accept a message longer than this.
 */
#define YYY_SAFE_IRC_MSG_LEN (((255 - 2) - 32) - sizeof("PRIVMSG :"))

/**
 * @brief Notes that a connection attempt is being made.
 *
 * This adds a visual indicator to the UI that a connection is in progress. It
 * must either be completed by a call to YYY_AddConnection or
 * YYY_FailedConnection with the same uri.
 *
 * @sa YYY_FailedConnection
 * @sa YYY_AddConnection
 */
void YYY_FASTCALL YYY_AttemptingConnection(const char *YYY_RESTRICT uri,
    void *YYY_RESTRICT arg);

/**
 * @brief Indicates a connection attempt failed.
 *
 * @sa YYY_AttemptingConnection
 */
void YYY_FASTCALL YYY_FailedConnection(const char *YYY_RESTRICT uri,
    void *YYY_RESTRICT arg);

/**
 * @brief Adds a new connection to the system.
 *
 * The connection should be valid, but not have any data read yet.
 * Must not be called from the main thread.
 *
 * @sa YYY_AttemptingConnection
 */
void YYY_FASTCALL YYY_AddConnection(
    struct YYY_NetworkSocket *YYY_RESTRICT socket,
    const char *YYY_RESTRICT uri,
    void *YYY_RESTRICT arg);

/**
 * @brief Sends a join message to the current server.
 */
void YYY_FASTCALL YYY_TryJoin(const char *channel);

/**
 * @brief Sends message to a channel.
 *
 * @param msg The message to send.
 * @param len Length of the message.
 * @param channel The channel to send to. A value of NULL indicates the 
 *    message should be sent to the channel that currently has focus.
 * @sa YYY_SendPrivateMessage
 */
void YYY_FASTCALL YYY_SendPrivateMessage(const char *msg,
    unsigned short len,
    const char *channel);

/**
 * @brief Sends a series of messages to a single channel.
 *
 * This is slightly more efficient than calling YYY_SendPrivateMessage for
 * each message if they are going to the same channel.
 *
 * @param msgs An array of messages
 * @param lens Lengths of the messages in @p msgs
 * @param num_msgs The length of the @p msgs and @p lens arrays.
 * @param channel The channel to send to. A value of NULL indicates the 
 *    messages should be sent to the channel that currently has focus.
 * @sa YYY_SendPrivateMessage
 */
void YYY_SendPrivateMessageV(const char **msgs,
    const unsigned short *lens,
    unsigned short num_msgs,
    const char *channel);

#ifdef __cplusplus
}
#endif

#endif KASHYYYK_YYY_MAIN_H
