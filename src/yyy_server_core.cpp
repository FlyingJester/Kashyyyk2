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

#include "yyy_server_core.hpp"

#include "yyy_server_ui.hpp"
#include "yyy_buffer.h"

#include "chat/yyy_chat.hpp"

#include "network/yyy_network.h"

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
  , m_channel(){
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

void ServerCore::firstConnected() const{
#ifndef NDEBUG
    assert(!m_first_connected);
    m_first_connected = true;
#endif
    
    
}
 
/*---------------------------------------------------------------------------*/

void ServerCore::handleMessage(const char *str, unsigned len){
    assert(len < YYY_MAX_MSG_LEN);
    
    Message msg;
    if(!m_protocol->parseMessage(str, len, msg)){
        puts("[ERROR parsing!]");
    }

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
        case eYYYChatNotification:
            {
                ChannelCore::ChannelMessage &yyy_msg = m_channel.pushFront();
                yyy_msg.m_type = ChannelCore::ChannelMessage::eNormalMessage;
                yyy_msg.m_message.assign(msg.m.notification.message,
                    msg.m.notification.message_len);
                YYY_DateSetNow(&yyy_msg.m_date);
            }
            break;
        default: break;
    }
}

/*---------------------------------------------------------------------------*/

void ServerCore::giveMessage(const char *msg, unsigned len){
    assert(len < YYY_MAX_MSG_LEN);
    YYY_PutMSGBuffer(m_buffer, msg, len);
    char buffer[YYY_MAX_MSG_LEN];
    while(unsigned long n = YYY_GetMSGBuffer(m_buffer, buffer)){
        handleMessage(buffer, n);
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
