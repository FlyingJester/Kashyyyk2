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

#ifndef YYY_CHANNEL_CONTROLLER_HPP
#define YYY_CHANNEL_CONTROLLER_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include "yyy_channel_core.hpp"
#include "yyy_channel_ui.hpp"

#include "monitor/yyy_monitor.hpp"

/*---------------------------------------------------------------------------*/

struct YYY_Message;

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

class ServerController;

/*---------------------------------------------------------------------------*/

class ChatProtocol;

/*---------------------------------------------------------------------------*/

class ChannelController {
    ChannelCore m_core;
    ChannelUI m_ui;

    Monitor m_monitor;

public:
    
    static void ChatScrollCallback(Fl_Widget *w, void *arg);

    void setup(const char *channel_name,
        unsigned channel_name_len,
        const std::string &server_name,
        ServerController &server_controller);

    inline const std::string &name() const { return m_core.name(); }

    void handleMessage(const YYY_Message &msg, bool is_mention, bool is_action);
    
    void select();
};

} // namespace YYY

#endif // YYY_CHANNEL_CONTROLLER_HPP
