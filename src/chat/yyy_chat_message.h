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

#ifndef YYY_CHAT_MESSAGE_H
#define YYY_CHAT_MESSAGE_H
#pragma once

/*---------------------------------------------------------------------------*/
    
enum YYY_MessageType {
    eYYYChatNotification,
    eYYYChatNick,
    eYYYChatIdentify,
    eYYYChatPing,
    eYYYChatPong,
    eYYYChatMessage,
    eYYYChatPart,
    eYYYChatJoin,
    eYYYChatQuit,
    eYYYChatBan,
    eYYYChatKick
};

/**
 * @brief POD structure representing a message.
 *
 * It is only intended as an intermediate format from a chat protocol to
 * Kashyyyk.
 */

struct YYY_Message {
    
    enum YYY_MessageType type;

    union {
        
        /** Used to simply read the from data. */
        struct {
            const char *from; /**< Sender's name */
            unsigned short from_len;
        } any_from;
        
        /** Used to read a message from data */
        struct {
            const char *from; /**< Sender's name */
            unsigned short from_len;
            const char *message;  /**< Message */
            unsigned short message_len;
        } any_message;
        
        struct {
            const char *from; /**< Sender's name */
            unsigned short from_len;
            const char *nick; /**< New nick */
            unsigned short nick_len;
        } nick;
        struct {
            const char *from; /**< Sender's name */
            unsigned short from_len;
            const char *user; /**< Username */
            unsigned short user_len;
            const char *real; /**< Realname */
            unsigned short real_len;
        } identify;
        struct {
            const char *from; /**< Sender's name */
            unsigned short from_len;
            const char *message; /**< Ping/pong message */
            unsigned short message_len;
        } ping, pong, quit;
        struct {
            const char *from; /**< Sender's name */
            unsigned short from_len;
            const char *message;/**< Message sent */
            unsigned short message_len;
            const char *to;  /**< Message destination (room or user) */
            unsigned short to_len;
        } message, notification;
        struct {
            const char *from; /**< Joiner/Parter's name */
            unsigned short from_len;
            const char *where; /**< Room the sender joined/parted */
            unsigned short where_len;
        } join, part;
        struct {
            const char *from; /**< User who performed the ban/kick */
            unsigned short from_len;
            const char *message; /** Ban/kick message */
            unsigned short message_len;
            const char *where; /**< Room the user was banned/kicked from */
            unsigned short where_len;
            const char *who; /**< User who was banned/kicked */
            unsigned short who_len;
        } ban, kick;
    }m;
};

#endif /* YYY_CHAT_MESSAGE_H */
