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

#ifndef YYY_CHAT_HPP
#define YYY_CHAT_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include "yyy_chat_message.h"

/*---------------------------------------------------------------------------*/

namespace YYY {

/**
 * @brief POD structure representing a message.
 *
 * It is only intended as an intermediate format from a chat protocol to
 * Kashyyyk.
 */
struct Message : public YYY_Message{
public:
    
    /* We could use a virtual destructor instead, but this allows us to use an
     * out-parameter and not do any allocations to have a protocol set what to
     * dealloc inside the message dynamically.
     */
    void (*m_free_callback)(YYY_Message &that);
    
    ~Message();
    Message()
      : m_free_callback((void (*)(YYY_Message &))0){}
};

/**
 * @brief Generic interface for chat protocols.
 *
 * Contains the notion of a server, a room, and a user. It is intended to be
 * usable to represent IRC, Discord, and Cisco Spark.
 */
class ChatProtocol {
public:
    
    virtual ~ChatProtocol();
    
    /**
     * @brief Parses a message in text to a YYY::Message structure
     *
     * The src buffer must remain valid for the members of @p out to be valid.
     *
     * @return true if the parse was successful.
     */
    virtual bool parseMessage(const char *src, size_t len, Message &out) = 0;
    
    /**
     * @brief Creates a string from a YYY::Message structure
     *
     * The returned string must be freed with freeMessageString().
     */
    virtual const char *messageToString(const Message &msg, size_t &out_len) = 0;
    
    const char *messageToString(const Message &msg);
    
    virtual void freeMessageString(const char *str) = 0;

    /**
     * @brief Creates a copy of a YYY::Message structure
     * 
     * The original src buffer for @p in must remain valid @p out.
     * The default implementation simply performs a by-value copy.
     * And implementation must properly handle when &in == &out.
     */
    virtual void copyMessage(const Message &in, Message &out);
    
    /**
     * @brief Creates a YYY::Message that correctly responds to a ping.
     * 
     * The default implementation copies in to out, and then changes the type
     * to eYYYPong.
     * And implementation must properly handle when &in == &out.
     */
    virtual void createResponseToPingMessage(const Message &in, Message &out);
    
    virtual bool compareIdentifiers(const char *str0, const char *str1, unsigned short len);

};

} // namespace YYY

#endif YYY_CHAT_HPP
