/* 
 *  Copyright (c) 2015-2017 Martin McDonough.  All rights reserved.
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

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

/*---------------------------------------------------------------------------*/

struct YYY_Monitor {
	pthread_cond_t cv;
	pthread_mutex_t mutex;
};

/*---------------------------------------------------------------------------*/

unsigned YYY_MonitorSize(){
    return sizeof(struct YYY_Monitor);
}

/*---------------------------------------------------------------------------*/

void YYY_InitMonitor(struct YYY_Monitor *monitor){
    int err;
    
    pthread_condattr_t cv_attr;
    pthread_mutexattr_t mx_attr;

    err = pthread_condattr_init(&cv_attr);
    err |= pthread_mutexattr_init(&mx_attr);
    assert(err==0);

    err = pthread_cond_init(&(monitor->cv), &cv_attr);
    err |= pthread_mutex_init(&(monitor->mutex), &mx_attr);
    assert(err==0);

    err = pthread_condattr_destroy(&cv_attr);
    err |= pthread_mutexattr_destroy(&mx_attr);
    assert(err==0);
    (void)err;
}

/*---------------------------------------------------------------------------*/

void YYY_DestroyMonitor(struct YYY_Monitor *monitor){
    int err;

    err = pthread_cond_destroy(&(monitor->cv));
    err |= pthread_mutex_destroy(&(monitor->mutex));
    assert(err==0);
    (void)err;
}

/*---------------------------------------------------------------------------*/

void YYY_LockMonitor(struct YYY_Monitor *monitor){
    pthread_mutex_lock(&(monitor->mutex));
}

/*---------------------------------------------------------------------------*/

void YYY_UnlockMonitor(struct YYY_Monitor *monitor){
    pthread_mutex_unlock(&(monitor->mutex));
}

/*---------------------------------------------------------------------------*/

void YYY_WaitMonitor(struct YYY_Monitor *monitor){
    const int err = pthread_cond_wait(&(monitor->cv), &(monitor->mutex));
    assert(err==0);
    (void)err;
}

/*---------------------------------------------------------------------------*/

void YYY_NotifyMonitor(struct YYY_Monitor *monitor){
    const int err = pthread_cond_signal(&(monitor->cv));
    assert(err==0);
    (void)err;
}

/*---------------------------------------------------------------------------*/

void YYY_NotifyAllMonitor(struct YYY_Monitor *monitor){
    const int err = pthread_cond_broadcast(&(monitor->cv));
    assert(err==0);
    (void)err;
}
