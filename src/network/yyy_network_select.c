/* 
 *  Copyright (c) 2016-2017 Martin McDonough.  All rights reserved.
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

/* Some big bullshit to deal with select not having an out-of-band way to
 * signal a break point, and Windows not having pipes be select()'able. */
#ifdef YYY_NETWORK_WIN32

    #include "yyy_network_win32.h"
    
    #include <WinSock2.h>
    #include <Windows.h>

    #define YYY_SELECT_ERR SOCKET_ERROR
    
    typedef HANDLE yyy_signaller_t;
    
    #define YYY_CREATE_SIGNALLER(OUT)\
        ((void)(OUT = CreateEvent(NULL, FALSE, FALSE, NULL)))
    
    #define YYY_DESTROY_SIGNALLER(SIG) ((void)CloseHandle(SIG))
    
    #define YYY_SIGNAL_SIGNALLER(SIG) ((void)SetEvent(SIG))
    
    #define YYY_CHECK_SIGNALLER(SIG)\
        (WaitForSingleObject((SIG), 0) == WAIT_OBJECT_0)
    
    #define YYY_CHECK_SIGNALLER_TIMED(SIG, TIME)\
        (WaitForSingleObject((SIG), (TIME / 1000) + 1) == WAIT_OBJECT_0)
    
    #define YYY_WAIT_SIGNALLER(SIG)\
        ((void)WaitForSingleObject((SIG), INFINITE))
    
#else
    
    #ifdef YYY_NETWORK_UNIX
        #include "yyy_network_unix.h"
    #endif
    
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/select.h>

    #include <stdlib.h>
    
    /* We use semaphores here because on Solaris 9/10 this means we do not need
     * pthreads or another library. Pthreads would be better, but if you are
     * in need of select you are most likely on Windows or dealing with ancient
     * Unix, and are 98% screwed for a good implementation of anything anyway.
     */
    #ifdef __linux__
        
        #define  _XOPEN_SOURCE 600
        
        #include <time.h>
        
    #endif
    
    #include <semaphore.h>
    
    #define YYY_SELECT_ERR (-1)
    
    typedef sem_t yyy_signaller_t;
    
    #define YYY_CREATE_SIGNALLER(OUT) ((void)sem_init(&(OUT), 0, 0))
    
    #define YYY_DESTROY_SIGNALLER(SIG) ((void)sem_destroy(&(SIG)))
    
    #define YYY_SIGNAL_SIGNALLER(SIG) ((void)sem_post(&(SIG)))
    
    #define YYY_CHECK_SIGNALLER(SIG) yyy_secret_check_signaller(&(SIG))
    
    #define YYY_CHECK_SIGNALLER_TIMED(SIG, TIME)\
        yyy_secret_check_signaller_timed(&(SIG), TIME)
    
    #define YYY_WAIT_SIGNALLER(SIG) ((void)sem_wait(&(SIG)))
    
    static unsigned yyy_secret_check_signaller(sem_t *sig) {
        const unsigned s = (sem_trywait(sig) == 0);
        if(s)
            while(sem_trywait(sig) == 0){}
        return s;
    }
    
    static unsigned yyy_secret_check_signaller_timed(sem_t *sig, unsigned t) {
    #ifdef __linux__
        struct timespec timespec;
        timespec.tv_sec = t / 1000000;
        timespec.tv_nsec = (t % 1000000) * 1000;
        return sem_timed_wait(sem, &timespec) == 0;
    #else
        long l = t;
        while(!yyy_secret_check_signaller(sig)){
            if(l == 0){
                return 0;
            }
            else if(l < 100000){
                usleep(l);
                l = 0;
            }
            else{
                usleep(100000);
                l -= 100000;
            }
        }
        return 1;
    #endif
    }
    
#endif
/* End bullshit. */

#include <assert.h>
#include <stdio.h>

/* Holds a group of YYY_NetworkSockets and associated user data.
 * Since select() modifies input, we have to reconstruct the socket set for
 * every query, so we just hold an array of YYY sockets and rebuild the set on
 * every wait.
 *
 * Poking is implemented using an extra socket.
 *
 * The socket array is stored in a manner that optimizes the case of just one
 * socket. If there is only one socket, then socket_capacity may be zero. This
 * indicates that sockets is in fact a pointer to the only socket, instead of a
 * pointer to an array of sockets.
 * It is still possible that (num_sockets == 1 && socket_capacity > 0), which
 * would happen if more than one socket was added (causing the array to be
 * created), but then sockets were removed until there was only one.
 *
 * It should always be the case that if socket_capacity is zero, num_sockets is
 * zero or one.
 */
struct YYY_SocketGroup{
    unsigned short num_sockets, socket_capacity; /* Array bookkeeping. */
    struct YYY_NetworkSocket **sockets; /* Sockets in the group. */
    void **user_data; /* user data corresponding to the sockets. */
    yyy_signaller_t signaller; /* Used for poking */
};

unsigned YYY_SocketGroupSize(){
    return sizeof(struct YYY_SocketGroup);
}

void YYY_InitSocketGroup(struct YYY_SocketGroup *group){
    group->sockets = NULL;
    group->user_data = NULL;
    group->num_sockets = 0;
    group->socket_capacity = 0;
    YYY_CREATE_SIGNALLER(group->signaller);
}

void YYY_DestroySocketGroup(struct YYY_SocketGroup *group){
    if(group->socket_capacity != 0){
        free(group->sockets);
        free(group->user_data);
    }
    YYY_DESTROY_SIGNALLER(group->signaller);
}

void YYY_AddSocketToGroup(struct YYY_NetworkSocket *socket, void *user_data,
    struct YYY_SocketGroup *group){
    
    /* Handle the zero-capacity optimization */
    if(group->socket_capacity == 0){
        assert(group->num_sockets <= 1);
        
        if(group->num_sockets == 1){
            /* Create the socket array and populate with the original socket
             * and the new socket.
             */
            struct YYY_NetworkSocket *const original_socket =
                (struct YYY_NetworkSocket *)(group->sockets);
            void *const original_data  = (void *)(group->user_data);
            group->sockets = malloc(sizeof(struct YYY_NetworkSocket *) * 4);
            group->sockets[0] = original_socket;
            group->user_data[0] = original_data;
            group->sockets[1] = socket;
            group->user_data[1] = user_data;
            group->num_sockets = 2;
            group->socket_capacity = 4;
        }
        else{
            if(group->num_sockets != 0){
                /* Uhhh... */
                assert(0&&"Invalid number of sockets for a zero-capacity group");
            }
            group->sockets = (struct YYY_NetworkSocket **)socket;
            group->num_sockets = 1;
            group->user_data = (void**)user_data;
        }
    }
    else{
        const unsigned short original_num_sockets = group->num_sockets++;
        assert(group->socket_capacity >= original_num_sockets);
        
        /* Bounds check and reallocation if needed. */
        if(group->socket_capacity <= original_num_sockets + 1){
            group->socket_capacity <<= 1;
            group->sockets = realloc(group->sockets,
                group->socket_capacity * sizeof(struct YYY_NetworkSocket *));
            group->user_data = realloc(group->user_data,
                group->socket_capacity * sizeof(void*));
        }
        group->sockets[original_num_sockets] = socket;
        group->sockets[original_num_sockets] = socket;
    }
}

enum YYY_NetworkError YYY_RemoveSocketFromGroup(struct YYY_NetworkSocket *socket,
    struct YYY_SocketGroup *group, void **out_user_data){
    unsigned short i = 0;
    if(group->num_sockets == 0)
        return eYYYNetworkFailure;
    
    if(group->socket_capacity == 0){
        assert(group->num_sockets <= 1);
        if(socket == (const struct YYY_NetworkSocket *)group->sockets){
            group->num_sockets = 0;
            out_user_data[0] = (void*)group->user_data;
            return eYYYNetworkSuccess;
        }
        else
            return eYYYNetworkFailure;
    }
    
    for(; i < group->num_sockets; i++){
        if(group->sockets[i] == socket){
            out_user_data[0] = group->user_data[i];
            group->num_sockets--;
            if(i != group->num_sockets){
                group->sockets[i] = group->sockets[group->num_sockets];
            }
            return eYYYNetworkSuccess;
        }
    }
    return eYYYNetworkFailure;
}

void YYY_PokeSocketGroup(struct YYY_SocketGroup *group){
    YYY_SIGNAL_SIGNALLER(group->signaller);
}

struct YYY_WaitResult {
    /* Ticks down from the max size, so that we don't need to store both the
     * length and an index. */
    unsigned short at;
    void *data[];
};

void YYY_DestroyWaitResult(struct YYY_WaitResult *res){
    free(res);
}

enum YYY_NetworkError YYY_GetNextWaitResult(struct YYY_WaitResult *r,
    void **out_user_data){

    if(r->at == 0){
        out_user_data[0] = NULL;
        return eYYYNetworkFailure;
    }
    else{
        r->at--;
        out_user_data[0] = r->data[r->at];
        return eYYYNetworkSuccess;
    }
}

static struct YYY_WaitResult *yyy_create_single_result(void *data){
    struct YYY_WaitResult *const result =
        malloc(sizeof(struct YYY_WaitResult)+sizeof(void*));
    result->at = 1;
    result->data[0] = data;
    return result;
}

static unsigned yyy_collect_results(fd_set *set,
    struct YYY_SocketGroup *group,
    struct YYY_WaitResult **out_result){
    unsigned short i;
    /* The number of sockets found */
    unsigned short found;
    /* In case there is only one result, it is cached here */
    void *user_data;
    const unsigned short num_sockets = group->num_sockets;
    /* Check for any sockets that are failed. If there are none, we will
        * check for sockets that can be read.
        */
    for(i = found = 0; i < num_sockets; i++){
        const struct YYY_NetworkSocket *const socket = group->sockets[i];
        if(FD_ISSET(socket->socket, set)){
            user_data = group->user_data[i];
            found++;
        }
    }

    /* Add all found sockets */
    if(found == 1){
        out_result[0] = yyy_create_single_result(user_data);
        return 1;
    }
    else if(found != 0){
        struct YYY_WaitResult *const result =
            malloc(sizeof(struct YYY_WaitResult)+(sizeof(void*)*found));
        result->at = 0;
        for(i = 0; i < num_sockets; i++){
            const struct YYY_NetworkSocket *const socket = group->sockets[i];
            if(FD_ISSET(socket->socket, set)){
                 result->data[result->at++] = group->user_data[i];
            }
        }
        out_result[0] = result;
        return 1;
    }
    return 0;
}

/* Only waits until data can be read, or until an error occurs. Returns 0 for
 * read, 1 for error.
 */
enum YYY_NetworkError YYY_WaitOnSocketGroup(struct YYY_SocketGroup *group,
    long timeout_in_microsecond,
    struct YYY_WaitResult **out_result){

    fd_set err, rd;
    socklen_t min_sock_num = 1;
    
    /* Check for a poke first thing. */
    if(YYY_CHECK_SIGNALLER(group->signaller))
        return eYYYNetworkPoked;
    
    FD_ZERO(&err);
    FD_ZERO(&rd);
    
    out_result[0] = NULL;
    
    /* TODO: LOCK! */

    /* Prepare the fd_set's with the sockets. */
    if(group->socket_capacity == 0){
        /* Handle zero-capacity optimizations. */
        if(group->num_sockets == 0){
            /* We are waiting on a poke. */
            /* TODO: UNLOCK! */
            const long t = timeout_in_microsecond;
            if(t < 0){
                YYY_WAIT_SIGNALLER(group->signaller);
                return eYYYNetworkPoked;
            }
            else if(t == 0){
                return YYY_CHECK_SIGNALLER(group->signaller) ?
                    eYYYNetworkPoked : eYYYNetworkTimeout;
            }
            else {
                return YYY_CHECK_SIGNALLER_TIMED(group->signaller, t) ?
                    eYYYNetworkPoked : eYYYNetworkTimeout;
            }
        }
        else{
            const struct YYY_NetworkSocket *const socket =
                (const struct YYY_NetworkSocket *)group->sockets;
            assert(group->num_sockets == 1);
            FD_SET(socket->socket, &err);
            FD_SET(socket->socket, &rd);
            min_sock_num = socket->socket + 1;
        }
    }
    else{
        unsigned i = 0;
        for(; i < group->num_sockets; i++){
            const struct YYY_NetworkSocket *const socket = group->sockets[i];
            FD_SET(socket->socket, &err);
            FD_SET(socket->socket, &rd);
            if(socket->socket + 1 > min_sock_num)
                min_sock_num = socket->socket + 1;
        }
    }
    
    /* TODO: UNLOCK! */
    
    {
        int err_no;
        long time_remaining = timeout_in_microsecond;
        if(timeout_in_microsecond >= 0){
            while(time_remaining){
                struct timeval time = {0, 100000};
                err_no = select(min_sock_num, &rd, NULL, &err, &time);
                time_remaining -= 100000;
                if(YYY_CHECK_SIGNALLER(group->signaller))
                    return eYYYNetworkPoked;
            }
        }
        else{
            do{
                struct timeval time = {0, 100000};
                err_no = select(min_sock_num, &rd, NULL, &err, &time);
                if(YYY_CHECK_SIGNALLER(group->signaller))
                    return eYYYNetworkPoked;
            } while(err_no == 0);
        }
        if(err_no == 0)
            return eYYYNetworkTimeout;
        if(err_no < 0)
            return eYYYNetworkFailure;
    }
    
    /* TODO: LOCK! Below, add unlocks! */
    if(group->socket_capacity == 0){
        /* No need to do any loops here, only one socket could succeed. */
        struct YYY_NetworkSocket *const socket =
            (struct YYY_NetworkSocket *)group->sockets;
        void *const user_data = (void*)group->user_data;
        out_result[0] = yyy_create_single_result(user_data);
        if(FD_ISSET(socket->socket, &rd))
            return eYYYNetworkSuccess;
        else{
            assert(FD_ISSET(socket->socket, &err));
            return eYYYNetworkDisconnected;
        }
    }
    else{
        /* Find which sockets were read or failed. */
        if(yyy_collect_results(&err, group, out_result))
            return eYYYNetworkDisconnected;
        if(yyy_collect_results(&rd, group, out_result))
            return eYYYNetworkSuccess;

        /* WHAT!? */
        return eYYYNetworkFatal;
    }
    return eYYYNetworkFailure;
}
