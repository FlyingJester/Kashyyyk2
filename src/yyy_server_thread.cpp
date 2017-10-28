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

#include "yyy_server_thread.hpp"

#include "yyy_server_controller.hpp"

#include "yyy_buffer.h" // For max msg len

#include "network/yyy_network.h"
#include "thread/yyy_thread.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#ifndef _WIN32
#include <unistd.h>
#define YYY_MILLISLEEP(X_) usleep(X_ << 10)
#else
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define YYY_MILLISLEEP Sleep
#endif

struct YYY_NetworkSocket;

namespace YYY {

/*---------------------------------------------------------------------------*/

ServerThread::ServerThread()
  : m_thread((YYY_Thread *)malloc(YYY_ThreadSize()))
  , m_socket_group((YYY_SocketGroup *)malloc(YYY_SocketGroupSize()))
  , m_locked_should_die(false)
  , m_monitor(){
    YYY_InitThread(m_thread, ThreadCallback);
    YYY_InitSocketGroup(m_socket_group);
}

/*---------------------------------------------------------------------------*/

ServerThread::~ServerThread(){
    m_monitor.lock();
    m_locked_should_die = true;
    m_monitor.unlock();
    
    YYY_PokeSocketGroup(m_socket_group);
    
    YYY_JoinThread(m_thread);
    
    YYY_DestroyThread(m_thread);
    YYY_DestroySocketGroup(m_socket_group);
    free(m_thread);
    free(m_socket_group);
}

/*---------------------------------------------------------------------------*/

void ServerThread::start(){
    m_locked_should_die = false;
    YYY_StartThread(m_thread, this);
}

/*---------------------------------------------------------------------------*/

bool ServerThread::shouldDie(){
    m_monitor.lock();
    const bool die = m_locked_should_die;
    m_monitor.unlock();
    return die;
}

/*---------------------------------------------------------------------------*/

void ServerThread::threadCallback(){
    m_locked_should_die = false;
    while(!shouldDie()){
        struct YYY_WaitResult *results;
        const YYY_NetworkError err =
            YYY_WaitOnSocketGroup(m_socket_group, -1l, &results);
        
        switch(err){
            case eYYYNetworkPoked:
                break; // We were signalled to test shouldDie/reload the group.
            case eYYYNetworkSuccess:
                {
                    void *user_data;
                    while(YYY_GetNextWaitResult(results, &user_data) == eYYYNetworkSuccess){
                        static_cast<ServerController*>(user_data)->messageReady();
                    }
                    YYY_DestroyWaitResult(results);
                }
                break;
            case eYYYNetworkFailure:
                // TODO: Handle a disconnected server.
                break;
            case eYYYNetworkTimeout:
                YYY_MILLISLEEP(10);
                break;
            default:
                assert(false);
                // TODO: Store some kind of status value.
                return;
                
        }
    }
}

/*---------------------------------------------------------------------------*/

void ServerThread::addServer(ServerController &server){
    server.addToSocketGroup(m_socket_group);
    YYY_PokeSocketGroup(m_socket_group);
}

/*---------------------------------------------------------------------------*/

void ServerThread::removeServer(const ServerController &server){
    server.removeFromSocketGroup(m_socket_group);
}

} // namespace YYY
