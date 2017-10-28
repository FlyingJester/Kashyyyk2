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

#ifndef KASHYYYK_YYY_SERVER_CORE_HPP
#define KASHYYYK_YYY_SERVER_CORE_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include "yyy_channel_core.hpp"
#include "yyy_maintainer.hpp"

#include "chat/yyy_chat.hpp"

#include "monitor/yyy_monitor.hpp"

#include <string>

/*---------------------------------------------------------------------------*/

struct YYY_NetworkSocket;

/*---------------------------------------------------------------------------*/

struct YYY_SocketGroup;

/*---------------------------------------------------------------------------*/

struct YYY_MSGBuffer;

/*---------------------------------------------------------------------------*/

struct YYY_IRC_Message;

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

class ServerUI;

/*---------------------------------------------------------------------------*/

class ChatProtocol;

/*---------------------------------------------------------------------------*/
/**
 * @brief Encapsulates the socket of a server.
 *
 * Keeps the socket private. This allows a child class (ServerSocketShare) to be friends with the
 * ServerController, but 
 */
class ServerSocketHolder {
private:
    YYY_NetworkSocket *m_socket;
protected:
    inline YYY_NetworkSocket *getSocket() const { return m_socket; }
    
    /**
     * @brief Sets the socket. Defined to allow a default constructor.
     *
     * Asserts that this object was default constructed and had not been
     * assigned before.
     */
    void setSocket(YYY_NetworkSocket *socket);
    
    ServerSocketHolder() : m_socket(NULL){}
    virtual ~ServerSocketHolder();
};

/*---------------------------------------------------------------------------*/
/**
 * @brief Encapsulates the socket of a server.
 *
 * Allows the ServerController to access the core's socket (by being a friend class) but keeps
 * everyone else out.
 */
class ServerSocketShare : private ServerSocketHolder {
protected:
    inline YYY_NetworkSocket *getSocket() const { return ServerSocketHolder::getSocket(); }
    
    /**
     * @brief Sets the socket. Defined to allow a default constructor.
     *
     * Asserts that this object was default constructed and had not been
     * assigned before.
     */
    inline void setSocket(YYY_NetworkSocket *socket) {
        ServerSocketHolder::setSocket(socket);
    }

    friend class ServerController;

    virtual ~ServerSocketShare();
};

/*---------------------------------------------------------------------------*/

class ServerCore : public ServerSocketShare {
    
    ChatProtocol *m_protocol;
    YYY_MSGBuffer *const m_buffer;
    
    void handleMessage(const char *value, unsigned len);

    std::string m_username, m_real;
    
    //! @brief Name of the server.
    std::string m_name;

    bool m_first_connected;

    bool m_messages_queued;
    
    /**
     * @brief Called when first connected to send handshake data to the server.
     *
     * Asserts that this is called only once.
     */
    void firstConnected();

public:
    ServerCore();
    virtual ~ServerCore();
    
    /**
     * @brief Sets the server name. Defined to allow a default constructor.
     *
     * Asserts that this object was default constructed and had not been
     * assigned before.
     */
    void setName(const std::string &name);
    
    /**
     * @brief Sets the server name. Defined to allow a default constructor.
     *
     * Asserts that this object was default constructed and had not been
     * assigned before.
     */
    void setName(const char *name, size_t name_len);
    
    //! @brief Performs initial setup operations on the core.
    inline void setup(const char *name, size_t name_len, ServerController &) {
        setName(name, name_len);
    }

    inline const std::string &name() const { return m_name; }
    
    /**
     * @brief Sets the protocol. Defined to allow a default constructor
     *
     * Asserts that this object was default constructed and had not been
     * assigned before.
     */
    void setProtocol(ChatProtocol &protocol);
    
    const ChatProtocol &getProtocol() const;

    /**
     * @brief Gets a message from the buffer, reading from the socket if none are available.
     *
     * When there is data to read from the socket, this should be called to get new messages until
     * it returns false.
     *
     * @p channel is valid until the next call to getMessage on this ServerCore. An empty string
     * (but not NULL) indicates the message should go to the server. A NULL means that no channel
     * should get the message.
     */
    bool getMessage(Message &out_message,
        const char *&channel,
        unsigned &channel_len,
        char *buffer,
        unsigned buffer_len);
    
    /**
     * @brief Notifies the server that it has become disconnected.
     * 
     * @warning This should NOT be  called from the main thread.
     */
    void notifyDisconnected();
    
    /**
     * @brief Notifies the server that it has been reconnected.
     * 
     * The server starts out assuming it is connected, so this is only needed
     * after a disconnect.
     * 
     * @warning This should NOT be  called from the main thread.
     */
    void notifyReconnected();
    
    // These exist to allow the server thread to add and remove server sockets
    // without using friend classes or exposing the ServerCore's socket.
    void addToSocketGroup(YYY_SocketGroup *to);
    void removeFromSocketGroup(YYY_SocketGroup *) const;
};

} // namespace YYY

#endif // KASHYYYK_YYY_SERVER_CORE_HPP
