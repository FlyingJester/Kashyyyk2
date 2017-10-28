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

#include "kernel/OS.h"

/*---------------------------------------------------------------------------*/

struct YYY_Thread {
    thread_id m_thread;
    void *m_arg, *m_value;
    yyy_thread_function_t m_callback;
};

/*---------------------------------------------------------------------------*/

static int32 yyy_thread_wrapper(void *data){
    YYY_Thread *const thr = static_cast<YYY_Thread*>(data);
    thr->m_value = thr->callback(thr->arg);
    return 0;
}

/*---------------------------------------------------------------------------*/

unsigned YYY_ThreadSize(){
    return sizeof(YYY_Thread);
}

/*---------------------------------------------------------------------------*/

void YYY_InitThread(YYY_Thread *thr, yyy_thread_function_t func){
    thr->m_callback = func;
    thr->m_thread =
        spawn_thread(yyy_thread_wrapper, "yyy_worker", B_NORMAL_PRIORITY, thr);
}

/*---------------------------------------------------------------------------*/

void YYY_StartThread(YYY_Thread *thr, void *data){
    thr->m_arg = data;
    thr->m_value = NULL;
    resume_thread(thr->m_thread);
}

/*---------------------------------------------------------------------------*/

void *YYY_JoinThread(YYY_Thread *thr){
    status_t i;
    wait_for_thread(thr->m_thread, &i);
    void *const value = thr->value;
    thr->m_value = NULL;
    return value;
}

/*---------------------------------------------------------------------------*/

void YYY_DestroyThread(YYY_Thread *thr){
    (void)thr;
}
