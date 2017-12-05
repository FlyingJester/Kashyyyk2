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

#include "yyy_channel_controller.hpp"

/*---------------------------------------------------------------------------*/

#include "kashyyyk2.hpp"

/*---------------------------------------------------------------------------*/

#include "chat/yyy_chat.hpp"

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

void ChannelController::ChatScrollCallback(Fl_Widget *w, void *arg){
    Fl_Valuator *const scroll = static_cast<Fl_Valuator *>(w);

    if(ChannelController *const channel = (ChannelController*)arg){
        ChannelCore &core = channel->m_core;
        channel->m_ui.updateChatWidget(core.messages(), core.numMessages(), false);
    }
}

/*---------------------------------------------------------------------------*/

void ChannelController::setup(const char *channel_name,
        unsigned channel_name_len,
        const std::string &server_name,
        ServerController &server_controller){
    
    assert(channel_name != NULL);

    m_core.name(channel_name, channel_name_len);
    m_ui.setup(channel_name, server_name.c_str(), *this);
}

/*---------------------------------------------------------------------------*/

void ChannelController::setupAsServerChannel(const char *server_name, unsigned server_name_len){
    m_core.name(server_name, server_name_len);
    m_ui.name(server_name, server_name_len);
}

void ChannelController::handleMessage(const YYY_Message &msg, bool is_mention, bool is_action){
    
    // Convert the YYY_Message into a ChannelMessage stored in the core, and add it to the UI.

    ChannelMessage &yyy_msg = m_core.pushFront();
    
    const ChannelMessage::Type type =
        (is_action && is_mention) ? ChannelMessage::eMentionAction :
        is_action ? ChannelMessage::eNormalAction :
        is_mention ? ChannelMessage::eMentionMessage :
        ChannelMessage::eNormalMessage;

    yyy_msg.type(type);
    yyy_msg.assignMessage(msg.m.any_message.message, msg.m.any_message.message_len);
    YYY_DateSetNow(&yyy_msg.m_date);
    m_ui.updateUI(m_core.messages(), m_core.numMessages(), true);

}


void ChannelController::select(){
    m_ui.select(m_core.messages(), m_core.numMessages());
    chat_scroll->callback(ChannelController::ChatScrollCallback, this);
}

} // namespace YYY
