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

#ifndef KASHYYYK_YYY_CHANNEL_MESSAGE_HPP
#define KASHYYYK_YYY_CHANNEL_MESSAGE_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include "yyy_date.h"

#include "utils/yyy_attributes.h"

/*---------------------------------------------------------------------------*/

namespace YYY {

/**
    * @brief Stores a single Chat message.
    *
    * This structure is optimized for size. Ultimately we should store this in a ringbuffer to
    * avoid the linked list we use.
    *
    * This is interpreted from a YYY_ChatMessage in the ServerCore, then is given to the channel.
    */
struct ChannelMessage {
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
        return IsInArray(messageLength());
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
        
    inline ChannelMessage()
        : m_user(NULL)
        , m_data(0){}

    ~ChannelMessage();
        
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

} // namespace YYY

/*---------------------------------------------------------------------------*/

extern "C" {

typedef YYY::MessageList YYY_MessageList;

/*---------------------------------------------------------------------------*/

void YYY_FreeMessageList(YYY_MessageList *list);

/*---------------------------------------------------------------------------*/

YYY_MessageList *YYY_GetMessageListElement(YYY_MessageList *list,
    unsigned element_index);

/*---------------------------------------------------------------------------*/

YYY_MessageList *YYY_GetMessageListTail(YYY_MessageList *list);

/*---------------------------------------------------------------------------*/

void YYY_INLINE_ALL_BODY_CALLS(
    YYY_TruncateMessageList(YYY_MessageList *list, unsigned len)
);

/*---------------------------------------------------------------------------*/

} // extern "C"

/*---------------------------------------------------------------------------*/

#endif // KASHYYYK_YYY_CHANNEL_MESSAGE_HPP
