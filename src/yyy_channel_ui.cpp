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

#include "ui/yyy_chat_widget.hpp"

#include <FL/Fl_Valuator.H>

#include "yyy_assert.h"

#include <assert.h>

namespace YYY {

/*---------------------------------------------------------------------------*/

ChannelUI::ChannelUI(ChannelCore &channel)
  : m_core(&channel){
    
}

/*---------------------------------------------------------------------------*/

void ChannelUI::setCore(ChannelCore &core){
    assert(m_core == NULL);
    m_core = &core;
}

/*---------------------------------------------------------------------------*/

ChannelCore &ChannelUI::getCore(){
    assert(m_core != NULL);
    return *m_core;
}

/*---------------------------------------------------------------------------*/

const ChannelCore &ChannelUI::getCore() const{
    assert(m_core != NULL);
    return *m_core;
}

/*---------------------------------------------------------------------------*/

bool ChannelUI::matches(const ChannelCore &c) const { return m_core == &c; }

/*---------------------------------------------------------------------------*/

void ChannelUI::updateScroll(Fl_Valuator &to) const {
    to.step(1);
    to.step(1.0);
    const unsigned n = m_core->numMessages();
    to.range(0.0, n);
    to.value(n);
}

/*---------------------------------------------------------------------------*/

void ChannelUI::updateChatWidget(ChatWidget &to,
    const Fl_Valuator &scroll) const {
    
    const unsigned max_lines = to.maxLines(),
        num_msg = m_core->numMessages(),
        maximum = (max_lines > num_msg) ? num_msg : max_lines;
    
    to.resize(maximum);
    
    const struct ChannelCore::MessageList *msg = m_core->messages();
    
    for(unsigned i = 0; i < maximum; i++){
        YYY_Assert(msg != NULL, "Channel message count is inconsistent");
        to.setMessageStatic(maximum - (i + 1), msg->m_message.m_user, msg->m_message.message());
        msg = msg->m_next;
    }
    to.redraw();
}
} // namespace YYY
