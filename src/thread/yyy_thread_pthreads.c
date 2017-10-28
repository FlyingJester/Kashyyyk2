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

#include "yyy_thread.h"

#include <pthread.h>

/*---------------------------------------------------------------------------*/

struct YYY_Thread{
    yyy_thread_function_t func;
    pthread_t thread;
};

/*---------------------------------------------------------------------------*/

unsigned YYY_ThreadSize(){
    return sizeof(struct YYY_Thread);
}

/*---------------------------------------------------------------------------*/

void YYY_InitThread(struct YYY_Thread *thr, yyy_thread_function_t func){
    thr->func = func;
}

/*---------------------------------------------------------------------------*/

void YYY_StartThread(struct YYY_Thread *thr, void *data){
    pthread_create(&thr->thread, NULL, thr->func, data);
}

/*---------------------------------------------------------------------------*/

void *YYY_JoinThread(struct YYY_Thread *thr){
    void *r;
    pthread_join(thr->thread, &r);
    return r;
}

/*---------------------------------------------------------------------------*/

void YYY_DestroyThread(struct YYY_Thread *thr){
    (void)thr;
}
