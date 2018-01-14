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

#include "yyy_server_controller.hpp"

#include "yyy_buffer.h"

#include "network/yyy_network.h"

#include "yyy_assert.h"

/*---------------------------------------------------------------------------*/

namespace YYY {

ChannelController *ServerController::findChannel(const char *channel_name, unsigned channel_name_len){
    for(Maintainer<ChannelController>::iterator i = m_channels.begin();
        i != m_channels.end(); i++){
        const std::string &iter_name = i->name();
        if(iter_name.length() == channel_name_len &&
            memcmp(channel_name, iter_name.c_str(), channel_name_len) == 0){
            return &(*i);
        }
    }
    return NULL;
}

void ServerController::addChannel(const char *channel_name, unsigned channel_name_len){
    ChannelController &channel = m_channels.create();
    channel.setup(channel_name, channel_name_len, m_core.name(), *this);
}

void ServerController::messageReady(){
    Message msg;
    const char *channel_name;
    unsigned channel_name_len;
    const std::string &name = m_core.name();
    const char *const name_str = name.c_str();

    assert(name.size() < ~((unsigned short)0));
    const unsigned short name_len = (unsigned short)name.size();
    
    char buffer[YYY_MAX_MSG_LEN];

    while(m_core.getMessage(msg, channel_name, channel_name_len, buffer, sizeof(buffer)-1)){
        // Dispatch the message to the appropriate channel.
        
        // If the channel is NULL, then we should just get another message.
        if(channel_name == NULL)
            continue;
        const ChatProtocol &protocol = m_core.getProtocol();
        const bool is_mention = protocol.isMention(name_str, name_len, msg),
            is_action = protocol.isAction(msg);

        // If the message is empty (but not NULL), then we should hand it to the server channel.
        if(channel_name_len == 0){
           m_channel.handleMessage(msg, is_mention, is_action);
        }
        else{
            // Find the receiving channel, if it exists.
            bool found = false;
            if(ChannelController *const channel = findChannel(channel_name, channel_name_len)){
                channel->handleMessage(msg, is_mention, is_action);
            }
            else{
                // Add the new channel if it doesn't exist yet.
                ChannelController &new_channel = m_channels.create();
                new_channel.setup(channel_name, channel_name_len, m_core.name(), *this);
                new_channel.handleMessage(msg, is_mention, is_action);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void ServerController::addToSocketGroup(YYY_SocketGroup *to){
    YYY_AddSocketToGroup(m_core.getSocket(), this, to);
}

/*---------------------------------------------------------------------------*/

void ServerController::removeFromSocketGroup(YYY_SocketGroup *group) const {
    void *data;
    const YYY_NetworkError err = YYY_RemoveSocketFromGroup(
        const_cast<YYY_NetworkSocket*>(m_core.getSocket()), group, &data);
    
    assert(err == eYYYNetworkSuccess);    
    // A bit ugly, but it's just for the assert.
    assert(const_cast<const void*>(data) == static_cast<const void*>(this));
}

/*---------------------------------------------------------------------------*/

void ServerController::setup(const char *server_name,
    unsigned server_name_len,
    YYY_NetworkSocket *socket){
    
    m_core.setup(server_name, server_name_len, *this);
    m_core.setSocket(socket);
    m_ui.setup(server_name, server_name_len, *this);
    m_channel.setupAsServerChannel(server_name, server_name_len);
}

/*---------------------------------------------------------------------------*/

void ServerController::select(const char *channel_name, unsigned channel_name_len){
    YYY_Assert((channel_name == NULL) == (channel_name_len == 0),
            "Selection of an empty channel or with an invalid name.");
    if(channel_name == NULL || channel_name_len == 0){
        m_channel.select();
    }
    else if(ChannelController *const channel = findChannel(channel_name, channel_name_len)){
        channel->select();
    }
    else{
        YYY_Assert(findChannel(channel_name, channel_name_len) != NULL,
            "Selected a channel that does not exist.");
    }
}

} // namespace YYY
