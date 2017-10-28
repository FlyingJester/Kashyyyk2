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

#include "yyy_channel_core.hpp"
#include "yyy_channel_ui.hpp"
#include "yyy_assert.h"

#include <assert.h>

#if ( defined _MSC_VER ) && _MSC_VER >= 1500
// This is needed for std::find for some reason.
#include <type_traits>
#endif

#include <algorithm>

namespace YYY {

/*---------------------------------------------------------------------------*/

const unsigned ChannelCore::c_default_max_messages = 312;

/*---------------------------------------------------------------------------*/

ChannelCore::ChannelCore(const std::string& name)
  : m_name(name)
  , m_max_messages(c_default_max_messages)
  , m_messages(NULL)
  , m_spare(NULL)
  , m_num_messages(0){
    
}

/*---------------------------------------------------------------------------*/

ChannelCore::ChannelCore(const char *name, unsigned name_len)
  : m_name(name, name_len)
  , m_max_messages(c_default_max_messages)
  , m_messages(NULL)
  , m_spare(NULL)
  , m_num_messages(0){
    
}

/*---------------------------------------------------------------------------*/

ChannelCore::~ChannelCore(){
    YYY_FreeMessageList(m_messages);
    if(m_spare != NULL)
        delete m_spare;
}

/*---------------------------------------------------------------------------*/

unsigned short ChannelCore::maxMessages(unsigned short m){
    assert(m_num_messages != 0 || m_messages != NULL);
    
    if(m_max_messages > m){
        YYY_TruncateMessageList(m_messages, m);
        m_num_messages = m;
    }
    return m_max_messages = m;
}

/*---------------------------------------------------------------------------*/

ChannelMessage &ChannelCore::pushFront(){
    assert((m_num_messages == 0) == (m_messages == NULL));
    
    struct MessageList *const new_msg =
        (m_spare != NULL) ? m_spare : new MessageList();
    m_spare = NULL;
    
    new_msg->m_next = m_messages;
    m_messages = new_msg;
    
    assert(m_num_messages <= m_max_messages);
    assert(m_max_messages != 0);
    if(m_num_messages == m_max_messages){
        struct MessageList *before_new_spare = new_msg->m_next;
        
        assert(before_new_spare->m_next != NULL);
        assert(m_spare == NULL);
        
        while(before_new_spare->m_next->m_next != NULL)
            before_new_spare = before_new_spare->m_next;
        
        m_spare = before_new_spare->m_next;
        
        before_new_spare->m_next = NULL;
    }
    else
        m_num_messages++;
    
    return new_msg->m_message;
}

/*---------------------------------------------------------------------------*/

Maintainer<std::string>::const_iterator ChannelCore::getUsersBegin() const{
    return m_users.cbegin();
}

/*---------------------------------------------------------------------------*/

Maintainer<std::string>::const_iterator ChannelCore::getUsersEnd() const{
    return m_users.cend();
}
    
/*---------------------------------------------------------------------------*/

void ChannelCore::addUser(const char *user, const size_t user_len){
#if (!defined NDEBUG ) && !(( defined _MSC_VER ) && ( _MSC_VER >= 1500 ))
    {
        const std::string user_str = std::string(user, user_len);
        YYY_Assert(std::find(m_users.begin(), m_users.end(), user_str) == m_users.cend(),
            "Tried to add a user that already exists!");
    }
#endif
    m_users.create().assign(user, user_len);
}

} // namespace YYY
