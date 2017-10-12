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
    
    //! @brief Gets the Channel UI object.
    ChannelUI *getUI() { return m_ui; }
    
    //! @brief Gets the Channel UI object.
    const ChannelUI *getUI() const { return m_ui; }
    
    /**
     * @brief Stores a single Chat message.
     *
     * This is interpreted from a YYY_ChatMessage in the ServerCore, then is given to the channel.
     */
    class ChannelMessage {
    public:

        struct YYY_Date m_date;
        const char *m_user; //!< Non-owning copy.
    private:
        //! If the length is greater than sizeof(char*) then this is a pointer to chars. Otherwise,
        //! the chars are placed in the array.
        union{
            char *m_ptr;
            char m_array[sizeof(char*)];
        } m_message;

        /**
         * @brief Holds the message length and type enum packed together.
         *
         * @warning Access only through type()/messageLength().
         *
         * The least significant six bits are type, most significant ten bits are message length.
         * This saves us two bytes per message. Right now this isn't a win since the compiler pads
         * the message size to the nearest 4 bytes in 32-bit mode, but it allows us to add two
         * more bytes to the message before we spill to a larger size.
         */
        uint16_t m_data;

        static inline bool IsInArray(unsigned short l){
            return l < sizeof(char*)-1;
        }

        inline bool isInArray() const {
            return messageLength() < sizeof(char*)-1;
        }

    public:

        enum Type {
            eNormalMessage,
            eMentionMessage,
            eNormalAction,
            eMentionAction,
            eJoin,
            ePart,
            eQuit
        };
        
        ChannelMessage()
            : m_user(NULL)
            , m_data(0){}

        ~ChannelMessage(){
            if(!isInArray())
                free(m_message.m_ptr);
        }
        
        inline unsigned short messageLength() const { return m_data >> 6; }

        //! @brief Assigns the message string.
        void assignMessage(const char *msg, unsigned short len);
        
        //! @brief Gets the message string and size.
        const char *message(unsigned short &len) const;
        
        //! @brief Gets the message string.
        inline const char *message() const { unsigned short l; return message(l); }

        inline Type type() const { return (Type)(m_data & 0x003F); }
        inline void type(Type type) {
            // More clear version:
            /*
            const unsigned short length_mask = 0xFFC0;
            const unsigned short masked_type = ((unsigned short)type) & 0x003F;
            m_data &= length_mask;
            m_data |= masked_type;
            */

            // Fewer operations:
            const unsigned short mask = 0xFFC0 | type;
            m_data &= mask;
        }
    };
    
    //! Used to store messages in a linked list.
    struct MessageList {
        ChannelMessage m_message;
        struct MessageList *m_next;
    };
    
    unsigned short maxMessages(unsigned short m);
    unsigned short maxMessages() const { return m_max_messages; }
    
    unsigned short numMessages() const { return m_num_messages; }
    
    //! Creates a new message in the front of the list, and returns a reference to it.
    ChannelMessage &pushFront();
    
    const struct MessageList *messages() const { return m_messages; }
    
    static const unsigned c_default_max_messages;
    
    const std::string& name() const { return m_name; }
    std::string &name() { return m_name; }
    void name(const std::string &name) { m_name = name; }
    void name(const char *name) { m_name = name; }
    
    //! @brief Used to iterate the current users.
    Maintainer<std::string>::const_iterator getUsersBegin() const;
    //! @brief Used to iterate the current users.
    Maintainer<std::string>::const_iterator getUsersEnd() const;
    
    void addUser(const char *user, const size_t user_len);
    inline void addUser(const std::string &user) { addUser(&(user[0]), user.length()); }

private:
    
    static void FreeList(struct MessageList *list);
    static struct MessageList *GetElement(struct MessageList *list,
        unsigned element_index);
    static struct MessageList *Tail(struct MessageList *list);
    static void YYY_INLINE_ALL_BODY_CALLS(
        TruncateList(struct MessageList *list, unsigned len)
    );
    
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
