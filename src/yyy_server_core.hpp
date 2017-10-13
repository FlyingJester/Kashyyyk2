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

class ServerCore {
    
    ChatProtocol *m_protocol;

    // Must not be a reference to allow us to have a default constructor so
    // that this can be held in a vector in older C++ standard compilers.
    ServerUI *m_ui; //!< Back reference to the UI
    Maintainer<ChannelCore> m_channels;
    
    YYY_NetworkSocket *m_socket;
    YYY_MSGBuffer *const m_buffer;
    Monitor m_monitor;
    
    void handleMessage(const char *value, unsigned len);
    
    //! @brief Holds the channel core for the server's notice channel
    ChannelCore m_channel;

    std::string m_username, m_real, m_name;

    bool m_first_connected;

public:
    ServerCore();
    ~ServerCore();
    
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

    inline const std::string &name() const { return m_name; }

    /**
     * @brief Sets the UI. Defined to allow a default constructor.
     *
     * Asserts that this object was default constructed and had not been
     * assigned before.
     */
    void setUI(ServerUI &ui);
    
    /**
     * @brief Gets the UI.
     */
    inline ServerUI *getUI() { return m_ui; }
    
    /**
     * @brief Gets the UI.
     */
    inline const ServerUI *getUI() const { return m_ui; }
    
    /**
     * @brief Sets the UI. Defined to allow a default constructor.
     *
     * Asserts that this object was default constructed and had not been
     * assigned before.
     */
    void setSocket(YYY_NetworkSocket *socket);
    
    /**
     * @brief Sets the protocol. Defined to allow a default constructor
     *
     * Asserts that this object was default constructed and had not been
     * assigned before.
     */
    void setProtocol(ChatProtocol &protocol);

    void createNewUi();
    
    inline YYY_NetworkSocket *getSocket() { return m_socket; }

    ChannelCore &serverChannel() { return m_channel; }
    const ChannelCore &serverChannel() const { return m_channel; }
    
    ChannelCore &addChannel(const char *name);
    
    /**
     * @brief Called when first connected to send handshake data to the server.
     *
     * Asserts that this is called only once.
     */
    void firstConnected();
    
    /**
     * @brief Gives the server core a new message.
     *
     * No parsing should be done prior. The server is capable of piecing
     * together spliced messages, and will buffer any fragments.
     */
    void giveMessage(const char *msg, unsigned len);
    
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
