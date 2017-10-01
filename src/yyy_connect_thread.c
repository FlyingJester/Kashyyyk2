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

#include "yyy_connect_thread.h"

#include "yyy_main.h"

#include "thread/yyy_thread.h"
#include "monitor/yyy_monitor_backend.h"
#include "network/yyy_network.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*---------------------------------------------------------------------------*/

struct yyy_connection {
    struct yyy_connection *next;
    unsigned port;
    void *arg;
    char url[];
};

/*---------------------------------------------------------------------------*/

static struct YYY_Thread *yyy_connect_thread = NULL;
static struct YYY_Monitor *yyy_connect_monitor = NULL;
static unsigned yyy_should_die = 0;
static struct yyy_connection *yyy_connection_list = NULL;

/*---------------------------------------------------------------------------*/

static struct YYY_NetworkSocket *yyy_do_connect(struct yyy_connection *conn){
    struct YYY_NetworkSocket *const socket = malloc(YYY_NetworkSocketSize());
    enum YYY_NetworkError err = YYY_InitSocket(socket);
    assert(err == eYYYNetworkSuccess);
    
    if((err = YYY_ConnectSocket(socket, conn->url, conn->port, 1000000)) != eYYYNetworkSuccess){
        YYY_DestroySocket(socket);
        free(socket);
        
        puts(YYY_ExplainError(err));
        fflush(stdout);
        
        return NULL;
    }
    
    
    return socket;
}

/*---------------------------------------------------------------------------*/

void *yyy_connect_thread_func(void *data){
    (void)data;

    puts("[yyy_connect_thread_func]starting");
yyy_connect_thread_begin:
    
    YYY_LockMonitor(yyy_connect_monitor);
    
    
yyy_connect_thread_wait:
    if(yyy_should_die){
        YYY_UnlockMonitor(yyy_connect_monitor);
        puts("[YYY_QueueConnection]ending");
        return NULL;
    }
    
    if(yyy_connection_list == NULL){
        puts("[yyy_connect_thread_func]Waiting...");
        YYY_WaitMonitor(yyy_connect_monitor);
        goto yyy_connect_thread_wait;
    }
    {
        struct yyy_connection *const conn = yyy_connection_list;
        yyy_connection_list = conn->next;
        YYY_UnlockMonitor(yyy_connect_monitor);
        
        puts("[yyy_connect_thread_func]Attempting connection");
        YYY_AttemptingConnection(conn->url, conn->arg);
        
        {
            struct YYY_NetworkSocket *const socket = yyy_do_connect(conn);
            if(socket != NULL){
                puts("[yyy_connect_thread_func]Adding connection");
                YYY_AddConnection(socket, conn->url, conn->arg);
            }
            else{
                puts("[yyy_connect_thread_func]Connection failed");
                YYY_FailedConnection(conn->url, conn->arg);
            }
            free(conn);
        }
    }
    goto yyy_connect_thread_begin;
}

/*---------------------------------------------------------------------------*/

void YYY_StartConnectThread(){
    
    yyy_connect_thread = malloc(YYY_ThreadSize());
    
    yyy_connection_list = NULL;
    
    yyy_should_die = 0;
    
    yyy_connect_monitor = malloc(YYY_MonitorSize());
    YYY_InitMonitor(yyy_connect_monitor);
    
    YYY_InitThread(yyy_connect_thread, yyy_connect_thread_func);
    YYY_StartThread(yyy_connect_thread, NULL);
}

/*---------------------------------------------------------------------------*/

void YYY_StopConnectThread(){
    
    /* Notify the thread to die. */
    YYY_LockMonitor(yyy_connect_monitor);

    yyy_should_die = 1;

    YYY_UnlockMonitor(yyy_connect_monitor);
    YYY_NotifyMonitor(yyy_connect_monitor);
    
    /* Join and free the thread */
    YYY_JoinThread(yyy_connect_thread);
    free(yyy_connect_thread);
    yyy_connect_thread = NULL;
    
    /* Clean up the monitor */
    YYY_DestroyMonitor(yyy_connect_monitor);
    free(yyy_connect_monitor);
    yyy_connect_monitor = NULL;
    
    { /* Free the connection list of any remaining connections. */
        struct yyy_connection *conn = yyy_connection_list;
        while(conn){
            struct yyy_connection *const next = conn->next;
            free(conn);
            conn = next;
        }
    }
}

/*---------------------------------------------------------------------------*/

// Should only be called on the main thread.
void YYY_QueueConnection(const char *url, unsigned port, void *arg){
    /* Construct the new connection request */
    const unsigned len = strlen(url);
    struct yyy_connection *const conn = malloc(sizeof(struct yyy_connection) + len + 1);
    puts("[YYY_QueueConnection]starting");
    conn->port = port;
    conn->next = NULL;
    conn->arg = arg;
    memcpy(conn->url, url, len + 1);
    
    /* Queue up the request */
    YYY_LockMonitor(yyy_connect_monitor);
    
    puts("[YYY_QueueConnection]appending");
    if(yyy_connection_list != NULL){
        /* Find the last connection in the list, and add this to the end of that. */
        struct yyy_connection *list = yyy_connection_list;
        while(list->next)
            list = list->next;
        
        list->next = conn;
    }
    else
        yyy_connection_list = conn;

    puts("[YYY_QueueConnection]notifying");

    YYY_UnlockMonitor(yyy_connect_monitor);
    YYY_NotifyMonitor(yyy_connect_monitor);
    puts("[YYY_QueueConnection]finishing");
}
