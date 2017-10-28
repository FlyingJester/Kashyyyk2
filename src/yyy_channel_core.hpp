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

#ifndef KASHYYYK_YYY_CHANNEL_CORE_HPP
#define KASHYYYK_YYY_CHANNEL_CORE_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include "yyy_channel_message.hpp"

#include "utils/yyy_maintainer.hpp"

#include <utility>
#include <string>
#include <vector>
#include <assert.h>

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

class ChannelUI;

/*---------------------------------------------------------------------------*/

class ChannelCore {
public:
    ChannelCore(const std::string& name);
    ChannelCore(const char *name = NULL, unsigned name_len = 0);
    ~ChannelCore();
    
    unsigned short maxMessages(unsigned short m);
    unsigned short maxMessages() const { return m_max_messages; }
    
    unsigned short numMessages() const { return m_num_messages; }
    
    //! Creates a new message in the front of the list, and returns a reference to it.
    ChannelMessage &pushFront();

    const MessageList *messages() const { return m_messages; }
    
    static const unsigned c_default_max_messages;
    
    const std::string& name() const { return m_name; }
    std::string &name() { return m_name; }
    void name(const std::string &name) { m_name = name; }
    void name(const char *name, unsigned len) { m_name.assign(name, len); }
    
    //! @brief Used to iterate the current users.
    Maintainer<std::string>::const_iterator getUsersBegin() const;
    //! @brief Used to iterate the current users.
    Maintainer<std::string>::const_iterator getUsersEnd() const;
    
    void addUser(const char *user, const size_t user_len);
    inline void addUser(const std::string &user) { addUser(&(user[0]), user.length()); }

private:
    
    //! @brief  Channel Name
    std::string m_name;
    
    //! @brief Channel Topic
    std::string m_topic;
    
    //! Maximum messages to store in m_messages before beginning to remove the last message.
    unsigned short m_max_messages;
    
    /**
     * @brief A vector of ref-counted user names that are contained in m_messages.
     *
     * Under normal circumstances, the vector is faster than a std::map.
     */
    std::vector<std::pair<const char *, unsigned> > m_message_users;
    
    /**
     * @brief Users currently in the channel.
     *
     * This is unrelated to m_message_users.
     */
    Maintainer<std::string> m_users;
    
    /// A list of messages
    MessageList *m_messages;
    /**
     * @brief A 'spare' list node to avoid extra allocations.
     *
     * This is used because the most common case is to remove the last node and
     * add one on the end. This allows us to not involve the allocator when 
     * these actions happen, even if they do not occur in the same function
     * call.
     */
    struct MessageList *m_spare;
    
    /// Size of m_messages
    unsigned m_num_messages;
};

} // namespace YYY

#endif // KASHYYYK_YYY_CHANNEL_CORE_HPP
