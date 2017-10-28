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

#ifndef YYY_SERVER_CONTROLLER_HPP
#define YYY_SERVER_CONTROLLER_HPP
#pragma once

#include "yyy_channel_core.hpp"
#include "yyy_channel_ui.hpp"
#include "yyy_channel_controller.hpp"
#include "yyy_server_core.hpp"
#include "yyy_server_ui.hpp"

#include "monitor/yyy_monitor.hpp"

#include "utils/yyy_maintainer.hpp"

#include <string>

namespace YYY {

class ChatProtocol;

class ServerController {

    Maintainer<ChannelController> m_channels;
    
    ServerCore m_core;

    ServerUI m_ui;

    //! @brief Holds the channel controller for the server's notice channel
    ChannelController m_channel;
    
    ChannelController *findChannel(const char *channel_name, unsigned channel_name_len);
    
    Monitor m_monitor;

public:
    
    ServerController(){}
    ServerController(ChatProtocol &protocol){ m_core.setProtocol(protocol); }

    //! @brief Signals the controller that a message is ready to be read.
    void messageReady();

    void addChannel(const char *channel_name, unsigned channel_name_len);
    inline void addChannel(const std::string &channel_name){
        addChannel(channel_name.c_str(), channel_name.length());
    }

    void setProtocol(ChatProtocol &protocol) { m_core.setProtocol(protocol); }
    void setSocket(YYY_NetworkSocket *socket) { m_core.setSocket(socket); }
    
    void addToSocketGroup(YYY_SocketGroup *group);
    void removeFromSocketGroup(YYY_SocketGroup *group) const;

    void setup(const char *server_name, unsigned server_name_len, YYY_NetworkSocket *socket);
    
    void select(const char *channel_name, unsigned channel_name_len);
    inline void select() { select(NULL, 0); }
};

} // namespace YYY

#endif // YYY_SERVER_CONTROLLER_HPP
