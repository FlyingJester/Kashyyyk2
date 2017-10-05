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

#include "yyy_chat.hpp"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

namespace YYY{

/*---------------------------------------------------------------------------*/

Message::~Message(){
    if(m_free_callback)
        m_free_callback(*this);
}

/*---------------------------------------------------------------------------*/

ChatProtocol::~ChatProtocol() {}

/*---------------------------------------------------------------------------*/

const char *ChatProtocol::messageToString(const Message &msg){
    size_t len;
    return messageToString(msg, len);
}

/*---------------------------------------------------------------------------*/
void ChatProtocol::copyMessage(const Message &in, Message &out){
    if(&in != &out)
        out = in;
}

/*---------------------------------------------------------------------------*/

void ChatProtocol::createResponseToPingMessage(const Message &in, Message &out){
    assert(in.type == eYYYChatPong);
    copyMessage(in, out);
    out.type = eYYYChatPong;
}

/*---------------------------------------------------------------------------*/

bool ChatProtocol::compareIdentifiers(const char *str0, const char *str1, unsigned short len){
    return memcmp(str0, str1, len) == 0;
}

} // namespace YYY
