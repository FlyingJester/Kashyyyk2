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

#ifndef KASHYYYK_YYY_SERVER_THREAD_HPP
#define KASHYYYK_YYY_SERVER_THREAD_HPP
#pragma once

/*---------------------------------------------------------------------------*/

#include "monitor/yyy_monitor.hpp"

/*---------------------------------------------------------------------------*/

struct YYY_Thread;

/*---------------------------------------------------------------------------*/

struct YYY_SocketGroup;

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

class ServerCore;

/*---------------------------------------------------------------------------*/

class ServerThread {
    YYY_Thread *const m_thread;
    YYY_SocketGroup *const m_socket_group;
    
    // TODO: Maybe make this into an atomic flag?
    bool m_locked_should_die;
    Monitor m_monitor;
    
    bool shouldDie();
    
    void threadCallback();
    static void *ThreadCallback(void *arg){
        static_cast<ServerThread*>(arg)->threadCallback();
        return (void*)0;
    }
    
public:
    ServerThread();
    ~ServerThread();
    
    void start();
    
    // This exists (in addition to ServerCore::addToSocketGroup) to keep the
    // group private and the server core to keep its socket private.
    void addServer(ServerCore &);
    void removeServer(const ServerCore &);
};

} // namespace YYY

#endif // KASHYYYK_YYY_SERVER_THREAD_HPP
