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

#include "yyy_channel_message.hpp"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

ChannelMessage::~ChannelMessage(){
    if(!isInArray())
        free(m_message.m_ptr);
}

/*---------------------------------------------------------------------------*/

void ChannelMessage::assignMessage(const char *msg, unsigned short len){
    m_data &= 3;
    m_data |= len << 6;
    char *dest;
    if(IsInArray(len))
        dest = m_message.m_array;
    else
        dest = m_message.m_ptr = (char *)malloc(len+1);
    assert(dest != NULL);
    memcpy(dest, msg, len);
    dest[len] = '\0';
    
}

/*---------------------------------------------------------------------------*/

const char *ChannelMessage::message(unsigned short &len) const {
    if(IsInArray(len = messageLength()))
        return m_message.m_array;
    else
        return m_message.m_ptr;
}

} // namespace YYY


/*---------------------------------------------------------------------------*/

void YYY_FreeMessageList(YYY_MessageList *list){
    while(list != NULL){
        YYY_MessageList *const to_delete = list;
        assert(to_delete != NULL);
        list = list->m_next;
        delete to_delete;
    }
}

/*---------------------------------------------------------------------------*/

YYY_MessageList *YYY_GetMessageListElement(YYY_MessageList *list,
    unsigned element_index){
    
    for(unsigned i = 0; i < element_index; i++){
        assert(list != NULL);
        list = list->m_next;
    }
    return list;
}

/*---------------------------------------------------------------------------*/

YYY_MessageList *YYY_GetMessageListTail(YYY_MessageList *list){
    if(list == NULL)
        return NULL;
    
    while(list->m_next != NULL)
        list = list->m_next;
    return list;
}

/*---------------------------------------------------------------------------*/

void YYY_TruncateMessageList(YYY_MessageList *list, unsigned len){
    struct YYY::MessageList *const msg = YYY_GetMessageListElement(list, len);
    YYY_FreeMessageList(msg->m_next);
    msg->m_next = NULL;
}

/*---------------------------------------------------------------------------*/
