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

#ifndef KASHYYYK_YYY_CHANNEL_UI_HPP
#define KASHYYYK_YYY_CHANNEL_UI_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include "ui/yyy_server_tree.hpp"

#include <stdlib.h>

#include <string>

/*---------------------------------------------------------------------------*/

class Fl_Valuator;

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

class ChatWidget;

/*---------------------------------------------------------------------------*/

class ChannelCore;

/*---------------------------------------------------------------------------*/

struct MessageList;

/*---------------------------------------------------------------------------*/

struct ChannelMessage;

/*---------------------------------------------------------------------------*/

class ChannelUI {

    ServerTree::ChannelData *m_ui_data;
    
    // Message at the bottom of the screen. This is zero for the default.
    unsigned m_at;

    std::string m_name;

    static void RedrawUI();
    bool isSelected() const;
    void updateTreeUIForLastMessage(const MessageList *messages) const;

public:

    ChannelUI()
      : m_ui_data(NULL)
      , m_at(0){
        
    }
    
    void setup(const char *channel_name,
        const char *server_name,
        ChannelController &controller);

    void updateScroll(unsigned num_messages) const;

    void updateChatWidget(const MessageList *messages,
        unsigned num_messages,
        bool new_msg) const;

    void updateForScroll(const MessageList *messages,
        unsigned num_messages) const;
    
    void updateUI(const MessageList *messages,
        unsigned num_messages,
        bool new_msg) const;

    void select(const MessageList *messages, unsigned num_messages);
    
    void name(const std::string &name);
    void name(const char *name, unsigned name_len);
};

} // namespace YYY

#endif // KASHYYYK_YYY_CHANNEL_UI_HPP
