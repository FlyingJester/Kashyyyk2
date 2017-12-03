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

// In debug mode, log all messages recieved.
#if ( defined _WIN32 ) && ( ( defined _DEBUG ) || ( defined DEBUG ) )

#define DEBUG_PRINT(STR, LEN) do {\
    char debug_buffer[81];\
    unsigned i;\
    debug_buffer[80] = '\0';\
    for(i = 0; i + 80 < LEN; i+=80){\
        memcpy(debug_buffer, STR + i, 80);\
        OutputDebugStringA(debug_buffer);\
    }\
    memcpy(debug_buffer, STR + i, LEN - i);\
    debug_buffer[LEN - i] = '\0';\
    OutputDebugStringA(debug_buffer);\
    OutputDebugString(TEXT("\n"));\
}while(0)

#elif ! ( ( defined NDEBUG ) || ( defined _WIN32 ) ) 

#define DEBUG_PRINT(STR, LEN) do {\
    fwrite(STR, 1, LEN, stdout);\
    putchar('\n');\
} while(0)

#else

#define DEBUG_PRINT(_0, _1) (void)(_0 + _1)

#endif

/*---------------------------------------------------------------------------*/

ServerSocketHolder::~ServerSocketHolder(){
    if(m_socket != NULL){
        YYY_CloseSocket(m_socket);
        YYY_DestroySocket(m_socket);
        free(m_socket);
    }
}
    
/*---------------------------------------------------------------------------*/

void ServerSocketHolder::setSocket(YYY_NetworkSocket *socket){
    assert(m_socket == NULL);
    m_socket = socket;
    YYY_MakeSocketNonBlocking(m_socket);
}

/*---------------------------------------------------------------------------*/

ServerSocketShare::~ServerSocketShare(){

}

/*---------------------------------------------------------------------------*/

ServerCore::ServerCore()
  : m_protocol(NULL)
  , m_buffer((YYY_MSGBuffer*)malloc(YYY_MSGBufferSize()))
  , m_username("KashyyykUser")
  , m_real("MilleniumYYY")
  , m_messages_queued(false){
    YYY_InitMSGBuffer(m_buffer);
    m_first_connected = false;
}

/*---------------------------------------------------------------------------*/

ServerCore::~ServerCore(){
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

void ServerCore::setProtocol(ChatProtocol &protocol){
    assert(m_protocol == NULL);
    m_protocol = &protocol;
}

/*---------------------------------------------------------------------------*/

const ChatProtocol &ServerCore::getProtocol() const{
    assert(m_protocol != NULL);
    return *m_protocol;
}

/*---------------------------------------------------------------------------*/

void ServerCore::firstConnected(){
    assert(!m_first_connected);
    m_first_connected = true;
    
    struct Configuration conf;
    conf.m_nick = m_username.c_str();
    conf.m_user = "KashyyykUser";
    conf.m_real = m_real.c_str();

    // Send the handshake.
    for(unsigned i = 0; i < m_protocol->getNumHelloMessages(); i++){
        Message msg;
        m_protocol->createHelloMessage(i, conf, msg);
        size_t len;
        const char *msg_str = m_protocol->messageToString(msg, len);
        YYY_WriteSocket(getSocket(), msg_str, len);
        m_protocol->freeMessageString(msg_str);
    }
}

/*---------------------------------------------------------------------------*/

bool ServerCore::getMessage(Message &out,
    const char *&channel,
    unsigned &channel_len,
    char *buffer,
    size_t buffer_len){
    
    // We set m_messages_queued to true if we parsed a message properly last call.

    if(!m_messages_queued){
        // Read available data from the socket.
        unsigned long len;
        YYY_ReadSocket(getSocket(), buffer, buffer_len, &len);
    
        assert(len < YYY_MAX_MSG_LEN);

        // Put the data into the buffer.
        YYY_PutMSGBuffer(m_buffer, buffer, len);
        
        if(m_first_connected == false)
            firstConnected();
    }

    // Read out of the buffer until no more messages are available.
    while(size_t len = YYY_GetMSGBuffer(m_buffer, buffer)){
        DEBUG_PRINT(buffer, len);
        m_protocol->parseMessage(buffer, len, out);

        YYY_MessageType type = out.type;

        // Check for a ping.
        if(type == eYYYChatPing){
            // Create a response to the ping
            m_protocol->createResponseToPingMessage(out, out);

            // Send the response.
            const char *const pong = m_protocol->messageToString(out, len);
            YYY_WriteSocket(getSocket(), pong, len);

            m_protocol->freeMessageString(pong);
            // Check for another message.
            continue;
        }

        // Set the destination channel, if any.
        switch(type){
            case eYYYChatJoin:
                channel = out.m.join.where;
                channel_len = out.m.join.where_len;
                break;
            case eYYYChatNotification:
                channel = "";
                channel_len = 0;
                break;
            case eYYYChatMessage:
                channel = out.m.message.to;
                channel_len = out.m.message.to_len;
                // If the message is addressed to us, then we change thge channel to the
                // sender's name.
                if(channel_len == m_username.length() &&
                    m_protocol->compareIdentifiers(m_username.c_str(),
                        channel,
                        channel_len)){
                    channel = out.m.any_from.from;
                    channel_len = out.m.any_from.from_len;
                }

                for(unsigned i = 0; i < channel_len; i++){
                    if(channel[i] == '/' || channel[i] == '!'){
                        channel_len = i;
                        break;
                    }
                }
                break;
            default:
                channel = NULL;
                channel_len = 0;
                break;
        }

        // Set m_messages_queued to true to indicate that it's possible more messages can be
        // found without reading from the socket again.
        return (m_messages_queued = true);
    }
    
    // We need more data to get more messages.
    return (m_messages_queued = false);
}

/*---------------------------------------------------------------------------*/

void ServerCore::send(const Message &msg){
    size_t len;
    const char *const msg_str = m_protocol->messageToString(msg, len);
    YYY_WriteSocket(getSocket(), msg_str, len);
    m_protocol->freeMessageString(msg_str);
}

} // namespace YYY
