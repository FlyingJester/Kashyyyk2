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

#include "yyy_channel_ui.hpp"
#include "yyy_channel_core.hpp"
#include "yyy_channel_message.hpp"

#include "ui/yyy_chat_widget.hpp"

#include "utils/yyy_fl_locker.hpp"

#include "kashyyyk2.hpp"

#include <FL/Fl_Valuator.H>

#include "yyy_assert.h"

#include <assert.h>

namespace YYY {

/*---------------------------------------------------------------------------*/

void ChannelUI::RedrawUI() {
    chat_scroll->redraw();
    chat_widget->redraw();
}

/*---------------------------------------------------------------------------*/

bool ChannelUI::isSelected() const{
    return m_ui_data == NULL ?
        server_tree->isServerSelected(m_name) :
        server_tree->isSelected(m_ui_data);
}

/*---------------------------------------------------------------------------*/

void ChannelUI::updateTreeUIForLastMessage(const MessageList *messages) const{
    if(messages != NULL){
        const YYY_MessageList *msg = messages;
        while(msg->m_next != NULL)
            msg = msg->m_next;
    }
}

/*---------------------------------------------------------------------------*/

void ChannelUI::setup(const char *channel_name,
    const char *server_name,
    ChannelController &controller){
    assert(m_name.empty());
    m_name = channel_name;
    m_ui_data = server_tree->addChannel(server_name, channel_name, &controller);
}

/*---------------------------------------------------------------------------*/

void ChannelUI::updateScroll(unsigned num_messages) const {
    chat_scroll->step(1);
    chat_scroll->step(1.0);
    chat_scroll->range(0.0, num_messages);
    chat_scroll->value(num_messages);
}

/*---------------------------------------------------------------------------*/

void ChannelUI::updateChatWidget(const MessageList *messages,
    unsigned num_messages,
    bool new_msg) const {
    
    const unsigned scroll_value = static_cast<unsigned>(chat_scroll->value());

    assert(scroll_value >= 0);
    assert(scroll_value <= num_messages);

    const unsigned skip_msgs = num_messages - scroll_value;
    
    const unsigned max_lines = chat_widget->maxLines();
    const unsigned maximum = (max_lines > (num_messages - skip_msgs)) ?
        (num_messages - skip_msgs) : max_lines;
    
    chat_widget->resize(maximum);

    // Skip over skip_msgs messages.
    for(unsigned i = 0; i < skip_msgs; i++){
        YYY_Assert(messages != NULL, "Channel message count is inconsistent");
        messages = messages->m_next;
    }

    for(unsigned i = 0; i < maximum; i++){
        YYY_Assert(messages != NULL, "Channel message count is inconsistent");
        chat_widget->setMessageStatic(maximum - (i + 1),
            messages->m_message.m_user,
            messages->m_message.message());
        messages = messages->m_next;
    }
}

void ChannelUI::updateUI(const MessageList *messages,
    unsigned num_messages,
    bool new_msg) const {
    FlLocker locker;
    if(isSelected()){
        updateScroll(num_messages);
        updateChatWidget(messages, num_messages, new_msg);
        RedrawUI();
    }
    else if(messages != NULL){
        // TODO: Handle what the type of message is for mentions.
    }
}

void ChannelUI::select(const MessageList *messages, unsigned num_messages){
    FlLocker locker;
    updateScroll(num_messages);
    updateChatWidget(messages, num_messages, false);
    RedrawUI();
}

void ChannelUI::name(const std::string &name){
    assert(m_name.empty());
    m_name = name;
}

void ChannelUI::name(const char *name, unsigned name_len){
    assert(m_name.empty());
    m_name.assign(name, name_len);
}

} // namespace YYY
