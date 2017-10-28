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

#include "yyy_network.h"
#include "yyy_network_unix.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/event.h>

#include <assert.h>
#include <string.h>

struct YYY_SocketGroup{
    int kq;
};

/*---------------------------------------------------------------------------*/

unsigned YYY_SocketGroupSize(){
    return sizeof(struct YYY_SocketGroup);
}

/*---------------------------------------------------------------------------*/

bool YYY_SocketGroupNeedsLocking(){
    return false;
}

/*---------------------------------------------------------------------------*/

void YYY_InitSocketGroup(struct YYY_SocketGroup *group){
    assert(group != NULL);
    group->kq = kqueue();
}

/*---------------------------------------------------------------------------*/

void YYY_DestroySocketGroup(struct YYY_SocketGroup *group){
    assert(group != NULL);
    close(group->kq);
}

/*---------------------------------------------------------------------------*/

void YYY_AddSocketToGroup(struct YYY_NetworkSocket *socket, void *user_data,
    struct YYY_SocketGroup *group){
    kevent ev;
    
    assert(group != NULL);
    assert(socket != NULL);
    
    EV_SET(&ev, socket->socket, EVFILT_READ, EV_ADD, 0, 0, user_data);
    kevent(group->kq, &ev, 1, NULL, 0, NULL);
}

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_RemoveSocketFromGroup(struct YYY_NetworkSocket *socket,
    struct YYY_SocketGroup *group, void **out_user_data){
    kevent ev;
    
    assert(group != NULL);
    assert(socket != NULL);
    
    EV_SET(&ev, socket->socket, EVFILT_READ, EV_DELETE, 0, 0, 0);
    kevent(group->kq, &ev, 1, NULL, 0, NULL);
}

/*---------------------------------------------------------------------------*/

void YYY_PokeSocketGroup(struct YYY_SocketGroup *group){
    kevent ev;
    
    assert(group != NULL);
    
    EV_SET(&ev, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, 0);
    kevent(group->kq, &ev, 1, NULL, 0, NULL);
}

/*---------------------------------------------------------------------------*/

#define YYY_KQUEUE_MAX_RESULTS 16

struct YYY_WaitResult{
    unsigned char index, length;
    void *user_results[];
}

/*---------------------------------------------------------------------------*/

void YYY_DestroyWaitResult(struct YYY_WaitResult *res){
    assert(r != NULL);
    assert(r->length != 0);
    assert(r->length <= YYY_KQUEUE_MAX_RESULTS);
    assert(r->index <= r->length);
    
    free(r);
}

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_GetNextWaitResult(struct YYY_WaitResult *r,
    void **out_user_data){
    
    assert(r != NULL);
    assert(out_user_data != NULL);
    
    assert(r->length != 0);
    assert(r->length <= YYY_KQUEUE_MAX_RESULTS);
    assert(r->index <= r->length);
    
    if(r->index == r->length)
        return eYYYNetworkFailure;
    
    out_user_data[0] = r->user_results[r->inedex++];
    return eYYYNetworkSuccess;
}

/*---------------------------------------------------------------------------*/

enum YYY_NetworkError YYY_WaitOnSocketGroup(struct YYY_SocketGroup *group,
    long timeout_in_microsecond,
    struct YYY_WaitResult **out_result){
    
    /* Receive any results from the queue. The one complication is that we only
     * a single type of result (read from a socket, or error on a socket), so
     * only results of a certain kind are returned from a single call.
     * Errors are favored for handling first.
     */
    
    kevent evs[YYY_KQUEUE_MAX_RESULTS];
    
    struct timespec time_local, *time = &time_local;
    memset(time, 0, sizeof(struct timespec));
    
    if(timeout_in_microsecond < 0){
        time = NULL;
    }
    else if(timeout_in_microsecond > 0){
        time_local.tv_sec = timeout_in_microsecond / 1000000;
        time_local.tv_nsec = timeout_in_microsecond * 1000;
    }
    
    {
        const int event_count = kevent(group->kq,
            NULL, 0,
            evs, YYY_KQUEUE_MAX_RESULTS,
            time);
        
        if(event_count < 0){
            assert(event_count == -1;
            return eYYYNetworkFatal;
        }
        if(event_count == 0){
            return eYYYNetworkTimeout;
        }
        else {
            
            /* Search for timer/pokes first. */
            unsigned event_num;
            
            assert(event_count > 0);
            assert(event_count <= YYY_KQUEUE_MAX_RESULTS);
            
            /* Check for a timer firing. */
            for(event_num = 0; event_num < event_count; event_num++){
                if(evs[event_num].filter == EVFILT_TIMER){
                    /* We only add poke timers as oneshots. */
                    assert((evs[event_num].flags & EV_ONESHOT) != 0);
                    retutn eYYYNetworkPoked;
                }
                else{
                    /* We only add poke timers as oneshots. */
                    assert((evs[event_num].flags & EV_ONESHOT) == 0);
                }
            }
            
            {
                /* There are reads or errors, so we should construct a result.
                 * Search for errors first.*/
                const unsigned short data_size = event_count * sizeof(void*);
                struct YYY_WaitResult *const result =
                    malloc(sizeof(struct YYY_WaitResult) + data_size);
                
                /* Set the starting length to zero. If this is still zero after
                 * searching for errors, we will simply add all evs to this
                 * result since they must all be reads. */
                unsigned short len = 0;
                result->index = 0;
                for(event_num = 0; event_num < event_count; event_num++){
                    if((evs[event_num].flags & EV_ERROR) != 0){
                        result->user_results[len++] = evs[event_num].udata;
                    }
                }
                /* If anything was found, then return the result, which is
                 * filled with any errors found */
                if(len != 0){
                    result->length = len;
                    out_result[0] = result;
                    return eYYYNetworkDisconnected;
                }
                
                /* Fill up the result with readable sockets. */
                for(event_num = 0; event_num < event_count; event_num++){
                    assert((evs[event_num].flags & EV_READ) != 0);
                    result->user_results[event_num] = evs[event_num].udata;
                }
                
                result->length = event_count;
                out_result[0] = result;
                return eYYYNetworkSuccess;
            }
        }
    }
}
