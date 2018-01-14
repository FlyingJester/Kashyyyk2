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

#include "yyy_callbacks.h"
#include "yyy_main.h"
#include "yyy_connect_thread.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* This is how many messages we will batch together for an over-long msg. */

#define YYY_SAY_VECTOR_LEN 16

/*---------------------------------------------------------------------------*/

static int yyy_is_whitespace(char c){
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

/*---------------------------------------------------------------------------*/

void YYY_Say(const char *text){
    const size_t msg_len = strlen(text);
    if(msg_len < YYY_SAFE_IRC_MSG_LEN){
        const char *msgs[YYY_SAY_VECTOR_LEN];
        unsigned short lens[YYY_SAY_VECTOR_LEN],
            msg_at = 0;
        size_t at = 0, old_at = 0;
        
        while(at != msg_len){
            assert(at < msg_len);
            do{
                msgs[msg_at] = text + at;

                if(at + YYY_SAFE_IRC_MSG_LEN <= msg_len){
                    lens[msg_at++] = YYY_SAFE_IRC_MSG_LEN;
                    at += YYY_SAFE_IRC_MSG_LEN;
                }
                else{
                    assert(msg_len - at < ~((unsigned short)0));
                    lens[msg_at++] = (unsigned short)(msg_len - at);
                    at = msg_len;
                    break;
                }
            }while(msg_at != YYY_SAY_VECTOR_LEN);

            YYY_SendPrivateMessageV(msgs, lens, msg_at, NULL);

            msg_at = 0;
        }
    }
    else{
        assert(msg_len < ~((unsigned short)0));
        YYY_SendPrivateMessage(text, (unsigned short)msg_len, NULL);
    }
}

/*---------------------------------------------------------------------------*/

void YYY_Connect(const char *uri, unsigned long port){
    if(port == 0)
        port = 6667;
    YYY_QueueConnection(uri, port, NULL);
}

/*---------------------------------------------------------------------------*/

void YYY_Join(const char *channel){
    YYY_TryJoin(channel);
}
