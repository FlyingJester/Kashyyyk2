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

#include "yyy_date.h"

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
    // Must not be a reference to allow us to have a default constructor so
    // that this can be held in a vector in older C++ standard compilers.
    ChannelUI *m_ui; //!< Back reference to the UI
public:
    ChannelCore(const std::string& name);
    ChannelCore(const char *name = NULL, unsigned name_len = 0);
    ~ChannelCore();
    
    //! Defined to allow a default constructor. Asserts that this object
    //! was default constructed and had not been assigned before.
    void setUI(ChannelUI &ui);
    
    //! Stores a single IRC message.
    struct Message {
        enum {
            eNormalMessage,
            eMentionMessage,
            eNormalAction,
            eMentionAction,
            eJoin,
            ePart,
            eQuit
        } m_type;
        
        struct YYY_Date m_date;
        const char *m_user;
        std::string m_message;
    };
    
    //! Used to store messages in a linked list.
    struct MessageList {
        struct Message m_message;
        struct MessageList *m_next;
    };
    
    unsigned short maxMessages(unsigned short m);
    unsigned short maxMessages() const { return m_max_messages; }
    
    unsigned short numMessages() const { return m_num_messages; }
    
    //! Creates a new message in the front of the list, and returns a reference to it.
    Message &pushFront();
    
    const struct MessageList *messages() const { return m_messages; }
    
    static const unsigned c_default_max_messages;
    
    const std::string name() const { return m_name; }
    std::string &name() { return m_name; }
    std::string &name(const std::string &name) { return m_name = name; }
    
private:
    
    static void FreeList(struct MessageList *list);
    static struct MessageList *GetElement(struct MessageList *list,
        unsigned element_index);
    static struct MessageList *Tail(struct MessageList *list);
    static void YYY_INLINE_ALL_BODY_CALLS(
        TruncateList(struct MessageList *list, unsigned len)
    );
    
private:
    
    /// Current username.
    ///
    /// TODO: This should be on the server?
    std::string m_name;
    
    /// Maximum messages to store in m_messages before beginning to remove the
    /// last message.
    unsigned short m_max_messages;
    
    /// A vector of ref-counted user names that are contained in m_messages.
    std::vector<std::pair<const char *, unsigned> > m_message_users;
    
    /// @brief Users currently in the channel.
    ///
    /// This is unrelated to m_message_users.
    std::vector<std::string> m_users;
    
    /// A list of messages
    struct MessageList *m_messages;
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
