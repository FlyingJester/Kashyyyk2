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

#include "yyy_irc_core.h"
#include "yyy_irc_numerics.h"
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static const char *const yyy_message_names[] = {
    "NOTICE",   /* eYYYChatNotification, */
    "NICK",     /* eYYYChatNick, */
    "USER",     /* eYYYChatIdentify, */
    "PING",     /* eYYYChatPing, */
    "PONG",     /* eYYYChatPong, */
    "PRIVMSG",  /* eYYYChatMessage, */
    "PART",     /* eYYYChatPart, */
    "JOIN",     /* eYYYChatJoin, */
    "QUIT",     /* eYYYChatQuit, */
    "BAN",      /* eYYYChatBan, */
    "KICK",     /* eYYYChatKick */
};

static const unsigned char yyy_message_lengths[] = {
    6,  /* eYYYChatNotification, */
    4,  /* eYYYChatNick, */
    4,  /* eYYYChatIdentify, */
    4,  /* eYYYChatPing, */
    4,  /* eYYYChatPong, */
    7,  /* eYYYChatMessage, */
    4,  /* eYYYChatPart, */
    4,  /* eYYYChatJoin, */
    4,  /* eYYYChatQuit, */
    3,  /* eYYYChatBan, */
    4   /* eYYYChatKick */
};

/*---------------------------------------------------------------------------*/

int YYY_IRC_CALL YYY_IRCParseMessage(const char *src,
    size_t len,
    struct YYY_Message *out_msg){
    
    size_t i;
    const size_t num_message_types = sizeof(yyy_message_names) / sizeof(void*);
    
#ifndef NDEBUG
    
    /* char is size 1 */
    assert(num_message_types == sizeof(yyy_message_lengths));
    
    for(i = 0; i < num_message_types; i++){
        assert(strlen(yyy_message_names[i]) == yyy_message_lengths[i]);
    }
#endif
    
    i = 0;
    while(i + 1 < len && src[i] == ' ')
        i++;
    
    /* Parse the from prefix, if any */
    if(src[i] == ':'){
        out_msg->m.any_from.from = src + (++i);
        while(i+1 < len && src[i] != ' ')
            i++;
        out_msg->m.any_from.from_len = (src + i) - out_msg->m.any_from.from;
        i++;
    }
    else{
        out_msg->m.any_from.from = NULL;
        out_msg->m.any_from.from_len = 0;
    }

    /* Parse the command type */
    if(i + 4 < len && YYY_STRING_IS_NUMERIC(src+i)){
        /* Numeric command */
        const enum YYY_NumericCode code =
            YYY_STRING_TO_NUMERIC_CODE(src+i);
        i += 3;

        /* TODO! 
        switch(code){
            

        }
        */
        while(i+1 < len && src[i] != ' ')
            i++;
        while(i+1 < len && src[i] == ' ')
            i++;

    }
    else{
        char type_buffer[10];
        unsigned at = 0, n;
        while(at < sizeof(type_buffer) && i + 1 < len && src[i] != ' ')
            type_buffer[at++] = src[i++];
        
        for(n = 0; n < num_message_types; n++){
            if(at == yyy_message_lengths[n] &&
                memcmp(type_buffer, yyy_message_names[n], at) == 0){
                out_msg->type = (enum YYY_MessageType)n;
                break;
            }
        }

        if(n == num_message_types)
            return 0;
    }

    /* Macros to save some typing when parsing IRC args */
#define YYY_IRC_FINISH_ARG(WHAT, TO)\
    do{\
        const unsigned short WHAT ## TO ## len = i;\
        out_msg->m.WHAT.TO = src + i;\
        while(i + 1 < len && !(src[i] == '\r' && src[i+1] == '\n'))\
            i++;\
        if(src[i] != '\r')\
            i = len;\
        out_msg->m.WHAT.TO ## _len = i - WHAT ## TO ## len;\
    }while(0)
#define YYY_IRC_GET_ARG(WHAT, TO)\
    do{\
        const unsigned short WHAT ## TO ## len = i;\
        out_msg->m.WHAT.TO = src + i;\
        while(i + 1 < len && src[i] != ' ' && !(src[i] == '\r' && src[i+1] == '\n'))\
            i++;\
        out_msg->m.WHAT.TO ## _len = i - WHAT ## TO ## len;\
    }while(0)

    /* Parse any arguments */
    while(i + 1 < len && src[i] == ' ')
        i++;
    switch(out_msg->type){
        case eYYYChatNotification: 
        case eYYYChatMessage:
            YYY_IRC_GET_ARG(notification, to);
                        /* If we don't find a colon, remember the final space character and
             * resest to that location */
            {
                unsigned last_space = ++i;
                while(i + 1 < len && !(src[i] == '\r' && src[i+1] == '\n')){
                    const char c = src[i++];
                    if(c == ':')
                        goto yyy_msg_found_colon;
                    if(c == ' ')
                        last_space = i;
                }

                /* If we made it this far, there was no colon. */
                i = last_space;
            }
yyy_msg_found_colon:
            if(i + 1 < len && src[i] == ':')
                i++;
            YYY_IRC_FINISH_ARG(notification, message);
            return 1;
        case eYYYChatQuit:
            if(src[i] == '\r' && src[i+1] == '\n')
                break;
            /* FALLTHROUGH */
        case eYYYChatPing: /* FALLTHROUGH */
        case eYYYChatPong:
            if(i + 1 < len && src[i] == ':')
                i++;
            YYY_IRC_FINISH_ARG(ping, message);
            return 1;
        case eYYYChatNick:
            if(i + 1 < len && src[i] == ':')
                i++;
            YYY_IRC_FINISH_ARG(nick, nick);
            return 1;
        case eYYYChatIdentify:
            YYY_IRC_GET_ARG(identify, user);

            /* If we don't find a colon, remember the final space character and
             * resest to that location */
            {
                unsigned last_space = ++i;
                while(i + 1 < len && !(src[i] == '\r' && src[i+1] == '\n')){
                    const char c = src[i++];
                    if(c == ':')
                        goto yyy_identify_found_colon;
                    if(c == ' ')
                        last_space = i;
                }

                /* If we made it this far, there was no colon. */
                i = last_space;
            }
yyy_identify_found_colon:
            YYY_IRC_FINISH_ARG(nick, nick);
            return 1;
        case eYYYChatJoin: /* FALLTHROUGH */
        case eYYYChatPart:
            if(i + 1 < len && src[i] == ':')
                i++;
            YYY_IRC_FINISH_ARG(join, where);
            return 1;
        case eYYYChatBan:
        case eYYYChatKick:
            return 0;
    }

    assert((out_msg->m.any_from.from == NULL) == (out_msg->m.any_from.from_len == 0));

    return 0;
}

/*---------------------------------------------------------------------------*/

const char *YYY_IRC_CALL YYY_IRCMessageToString(const struct YYY_Message *msg,
    size_t *out_len){
    
    const enum YYY_MessageType type = msg->type;
    
    const char *const msg_name = yyy_message_names[(unsigned)type];
    const unsigned char msg_name_len = yyy_message_lengths[(unsigned)type];
    
    /* Get the msg name length. Add 1 for a space, and 2 for crlf. */
    unsigned short msg_len = msg_name_len + 1 + 2;
    
    const char *from = NULL;
    const unsigned short from_len = msg->m.any_from.from_len,
        any_msg_len = msg->m.any_message.message_len;;
    
#ifndef NDEBUG
    const size_t num_message_types = sizeof(yyy_message_names) / sizeof(void*);
    assert((unsigned)type < num_message_types);
#endif

    /* Determine if there is a from prefix. */
    if(from_len){
        from = msg->m.any_from.from;
        /* Add one for the initial colon and one for the following space. */
        msg_len += 2 + from_len;
    }
    
    /* Determine the length of the message */
    
    switch(msg->type){
        case eYYYChatNotification: /* FALLTHROUGH */
        case eYYYChatMessage:
            msg_len += msg->m.message.to_len + 1;
        case eYYYChatQuit: 
            /* Quit can have zero args */
            if(any_msg_len == 0)
                break;
            /* Add one extra for the leading colon on the last arg. */
            msg_len++;
            /* FALLTHROUGH */
        case eYYYChatPing: /* FALLTHROUGH */
        case eYYYChatPong:
            msg_len += any_msg_len;
            break;
        case eYYYChatNick:
            msg_len += msg->m.nick.nick_len;
            break;
        case eYYYChatIdentify:
            /* Add one for the space and one for a leading colond on the last
             * arg. */
            msg_len += msg->m.identify.user_len + 2 +
                msg->m.identify.real_len;
            break;
        case eYYYChatJoin: /* FALLTHROUGH */
        case eYYYChatPart:
            msg_len += msg->m.join.where_len;
            break;
        case eYYYChatBan:
        case eYYYChatKick:
            msg_len += msg->m.kick.where_len;
            if(msg->m.kick.message_len){
                /* Add one for the space and one for a leading colond on the
                 * last arg. */
                msg_len += msg->m.kick.message_len + 2;
            }
            break;
    }
    
    /* Construct the message. */
    {
        char *const out_message = malloc(msg_len);
        unsigned short at = 0;
        out_len[0] = msg_len;
        
        /* Add the from prefix if present. */
        if(from_len){
            out_message[at++] = ':';
            memcpy(out_message+at, from, from_len);
            at += from_len;
            out_message[at++] = ' ';
        }
        
        /* Add the message type name */
        memcpy(out_message + at, msg_name, msg_name_len);
        at += msg_name_len;
        
        switch(msg->type){
            case eYYYChatNotification: /* FALLTHROUGH */
            case eYYYChatMessage:
                {
                    const unsigned short to_len = msg->m.message.to_len;
                    memcpy(out_message + at, msg->m.message.to, to_len);
                    at += to_len;
                    out_message[at++] = ' ';
                }
                /* FALLTHROUGH */
            case eYYYChatQuit: 
                /* Quit can have zero args */
                if(any_msg_len == 0)
                    break;
                /* FALLTHROUGH */
                out_message[at++] = ':';
                /* FALLTHROUGH */
            case eYYYChatPing: /* FALLTHROUGH */
            case eYYYChatPong:
                memcpy(out_message + at, msg->m.any_message.message, any_msg_len);
#ifndef NDEBUG
                at += any_msg_len;
#endif
                break;
        
            case eYYYChatNick:
                {
                    const unsigned short nick_len = msg->m.nick.nick_len;
                    memcpy(out_message + at, msg->m.nick.nick, nick_len);
#ifndef NDEBUG
                    at += nick_len;
#endif
                }
                break;
                
            case eYYYChatIdentify:
                {
                    const unsigned short user_len = msg->m.identify.user_len,
                        real_len = msg->m.identify.real_len;
                    memcpy(out_message + at, msg->m.identify.user, user_len);
                    at += user_len;
                    out_message[at++] = ' ';
                    out_message[at++] = ':';
                    memcpy(out_message + at, msg->m.identify.real, real_len);
#ifndef NDEBUG
                    at += real_len;
#endif
                }
                break;
                
            case eYYYChatJoin: /* FALLTHROUGH */
            case eYYYChatPart:
                {
                    const unsigned short where_len = msg->m.join.where_len;
                    memcpy(out_message + at, msg->m.join.where, where_len);
#ifndef NDEBUG
                    at += where_len;
#endif
                }
                break;
            case eYYYChatBan:
            case eYYYChatKick:
                {
                    const unsigned short where_len = msg->m.kick.where_len,
                        who_len = msg->m.kick.who_len;
                    memcpy(out_message + at, msg->m.kick.where, where_len);
                    at += where_len;
                    out_message[at++] = ' ';
                    memcpy(out_message + at, msg->m.kick.who, who_len);
                    at += who_len;
                    if(any_msg_len){
                        out_message[at++] = ' ';
                        out_message[at++] = ':';
                        memcpy(out_message + at, msg->m.any_message.message, any_msg_len);
#ifndef NDEBUG
                        at += any_msg_len;
#endif
                    }
                }
                break;
        }
        
        /* At was only properly updated at the very end if we are in debug */
        assert(at + 2 == msg_len);
        
        out_message[msg_len - 2] = '\r';
        out_message[msg_len - 1] = '\n';
        
        return out_message;
    }
}

