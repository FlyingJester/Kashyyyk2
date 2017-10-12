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

#include "kashyyyk2.hpp"

#include "yyy_server_core.hpp"

#include "yyy_server_ui.hpp"
#include "yyy_buffer.h"

#include "chat/yyy_chat.hpp"

#include "network/yyy_network.h"

#include "utils/yyy_fl_locker.hpp"

#include <stdlib.h>
#include <assert.h>

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

ServerCore::ServerCore()
  : m_protocol(NULL)
  , m_ui(NULL)
  , m_socket(NULL)
  , m_buffer((YYY_MSGBuffer*)malloc(YYY_MSGBufferSize()))
  , m_channel()
  , m_username("KashyyykUser")
  , m_real("MilleniumYYY"){
    YYY_InitMSGBuffer(m_buffer);
#ifndef NDEBUG
    m_first_connected = false;
#endif
}

/*---------------------------------------------------------------------------*/

ServerCore::~ServerCore(){
    
    if(m_socket != NULL){
        YYY_CloseSocket(m_socket);
        YYY_DestroySocket(m_socket);
        free(m_socket);
    }
    YYY_DestroyMSGBuffer(m_buffer);
    
    free(m_buffer);
}

/*---------------------------------------------------------------------------*/

void ServerCore::setName(const std::string &name){
    assert(m_name.empty());
    m_name = name;
}

/*---------------------------------------------------------------------------*/

void ServerCore::setName(const char *name, size_t name_len){
    assert(m_name.empty());
    m_name.assign(name, name_len);
}
    
/*---------------------------------------------------------------------------*/

void ServerCore::setUI(ServerUI &ui){
    assert(m_ui == NULL);
    m_ui = &ui;
    m_channel.setUI(ui.serverChannel());
}

/*---------------------------------------------------------------------------*/

void ServerCore::setSocket(YYY_NetworkSocket *socket){
    assert(m_socket == NULL);
    m_socket = socket;
}

/*---------------------------------------------------------------------------*/

void ServerCore::setProtocol(ChatProtocol &protocol){
    assert(m_protocol == NULL);
    m_protocol = &protocol;
}

/*---------------------------------------------------------------------------*/

void ServerCore::createNewUi(){
    assert(m_ui == NULL);
    ServerUI *const ui = new ServerUI(*this);
    m_ui = ui;
    m_channel.setUI(ui->serverChannel());
}

/*---------------------------------------------------------------------------*/

ChannelCore &ServerCore::addChannel(const char *name){
    ChannelCore &channel = m_channels.create();
    channel.name(name);
    return channel;
}

/*---------------------------------------------------------------------------*/

void ServerCore::firstConnected(){
    assert(!m_first_connected);
    m_first_connected = true;
    
    struct Configuration conf;
    conf.m_nick = m_username.c_str();
    conf.m_user = "KashyyykUser";
    conf.m_real = m_real.c_str();

    for(unsigned i = 0; i < m_protocol->getNumHelloMessages(); i++){
        Message msg;
        m_protocol->createHelloMessage(i, conf, msg);
        size_t len;
        const char *msg_str = m_protocol->messageToString(msg, len);
        YYY_WriteSocket(m_socket, msg_str, len);
        m_protocol->freeMessageString(msg_str);
    }
}

/*---------------------------------------------------------------------------*/

void ServerCore::handleMessage(const char *str, unsigned len){
    assert(len < YYY_MAX_MSG_LEN);

    Message msg;
    if(!m_protocol->parseMessage(str, len, msg)){
        puts("[ERROR parsing!]");
    }

    ChannelCore *dest = NULL;

    switch(msg.type){
        case eYYYChatPing:
            m_protocol->createResponseToPingMessage(msg, msg);
            {
                size_t len;
                const char *const pong = m_protocol->messageToString(msg, len);
                YYY_WriteSocket(m_socket, pong, len);
                m_protocol->freeMessageString(pong);
            }
            break;
        case eYYYChatJoin:
            // Check if this is a join for one of our current channels, or a confirmation of a
            // previous join attempt.
            {
                bool found = false;
                for(Maintainer<ChannelCore>::iterator iter = m_channels.begin();
                    iter != m_channels.cend(); iter++){
                    const std::string &name = iter->name();
                    if(name.length() == msg.m.join.where_len &&
                        memcmp(&(name[0]), msg.m.join.where, msg.m.join.where_len) == 0){
                        // This is a join for a channel we are already in.
                        iter->addUser(msg.m.join.from, msg.m.join.from_len);
                        found = true;
                        break;
                    }
                }
            }

            break;
        case eYYYChatNotification:
            dest = &m_channel;
        case eYYYChatMessage:
            {
                const char *const message = msg.m.message.message;
                const unsigned short message_len = msg.m.message.message_len;

                ChannelCore::ChannelMessage::Type type =
                    ChannelCore::ChannelMessage::eNormalMessage;

                // Find the destination channel.
                if(dest == NULL){
                    
                    // We are checking for the username in the message in this NULL check because
                    // this means we will only check if for messages, not notifications.
                    assert(msg.type == eYYYChatMessage);

                    // Check if our username exists in the message.
                    if(!m_username.empty()){
                        const char *const name = m_username.c_str();
                        const unsigned short name_len = (unsigned short)m_username.length();
                        for(unsigned short i = 0; i + name_len < message_len; i++){
                            if(m_protocol->compareIdentifiers(name, message, name_len)){
                                type = ChannelCore::ChannelMessage::eMentionMessage;
                                break;
                            }
                        }
                    }

                    const char *const from = msg.m.any_from.from;
                    const unsigned short from_len = msg.m.any_from.from_len;
                    for(Maintainer<ChannelCore>::iterator iter = m_channels.begin();
                        iter != m_channels.end(); iter++){
                        const std::string &channel_name = iter->name();
                        if(channel_name.length() == from_len &&
                            m_protocol->compareIdentifiers(channel_name.c_str(), from, from_len)){
                            dest = &(*iter);
                            break;
                        }
                    }

                    // We either reached the end, or we found the channel.
                    if(dest == NULL){
                        // TODO: Add this new channel?
                        break;
                    }
                }

                // We found the destination.
                assert(dest != NULL);
                ChannelCore::ChannelMessage &yyy_msg = dest->pushFront();

                yyy_msg.type(type);
                yyy_msg.assignMessage(message, message_len);
                YYY_DateSetNow(&yyy_msg.m_date);

                const std::string &channel_name = dest->name();
                if(server_tree->isSelected(m_name, channel_name)){
                    ChannelUI &channel = m_ui->serverChannel();
                    channel.updateScroll(*chat_scroll);
                    channel.updateChatWidget(*chat_widget, *chat_scroll);
                    chat_scroll->redraw();
                    chat_widget->redraw();
                    chat_widget->parent()->redraw();
                    
                }
            }
            break;
        default:
#ifndef NDEBUG
            { // Debugging target.
                volatile int i = 0;
                i = 1;
            }
#endif
            break;
    }
    
    if(m_first_connected == false)
        firstConnected();
}

/*---------------------------------------------------------------------------*/

void ServerCore::giveMessage(const char *msg, unsigned len){
    assert(len < YYY_MAX_MSG_LEN);
    YYY_PutMSGBuffer(m_buffer, msg, len);
    char buffer[YYY_MAX_MSG_LEN];
    unsigned long n = YYY_GetMSGBuffer(m_buffer, buffer);
    if(n){
        YYY::FlLocker locker;
        do{
            handleMessage(buffer, n);
        }while(n = YYY_GetMSGBuffer(m_buffer, buffer));
    }
}

/*---------------------------------------------------------------------------*/

void ServerCore::addToSocketGroup(YYY_SocketGroup *to){
    YYY_AddSocketToGroup(m_socket, this, to);
}

/*---------------------------------------------------------------------------*/

void ServerCore::removeFromSocketGroup(YYY_SocketGroup *group) const {
    void *data;
    const YYY_NetworkError err = YYY_RemoveSocketFromGroup(
        const_cast<YYY_NetworkSocket*>(m_socket), group, &data);
    
    assert(err == eYYYNetworkSuccess);    
    // A bit ugly, but it's just for the assert.
    assert(const_cast<const void*>(data) == static_cast<const void*>(this));
}

} // namespace YYY
