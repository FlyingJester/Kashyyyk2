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

#include "yyy_monitor_backend.h"

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x400
#include <stdlib.h>
#include <Windows.h>
#include <assert.h>
#undef WIN32_LEAN_AND_MEAN

/*---------------------------------------------------------------------------*/

struct YYY_Monitor {
	HANDLE ev;
	HANDLE mx;
};

/*---------------------------------------------------------------------------*/

unsigned YYY_MonitorSize(){
    return sizeof(struct YYY_Monitor);
}

/*---------------------------------------------------------------------------*/

void YYY_InitMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    monitor->ev = CreateEvent(NULL, FALSE, FALSE, NULL);
	monitor->mx = CreateMutex(NULL, FALSE, NULL);
}

/*---------------------------------------------------------------------------*/

void YYY_DestroyMonitor(struct YYY_Monitor *monitor){
	assert(monitor);
    CloseHandle(monitor->ev);
    CloseHandle(monitor->mx);
}

/*---------------------------------------------------------------------------*/

void YYY_LockMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    WaitForSingleObject(monitor->mx, INFINITE);
}

/*---------------------------------------------------------------------------*/

void YYY_UnlockMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
	ReleaseMutex(monitor->mx);
}

/*---------------------------------------------------------------------------*/

void YYY_WaitMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
	ReleaseMutex(monitor->mx);
    WaitForSingleObject(monitor->ev, INFINITE);
}

/*---------------------------------------------------------------------------*/

void YYY_NotifyMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    WaitForSingleObject(monitor->mx, INFINITE);
    SetEvent(monitor->ev);
	ReleaseMutex(monitor->mx);
}

/*---------------------------------------------------------------------------*/

void YYY_NotifyAllMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    WaitForSingleObject(monitor->mx, INFINITE);
    PulseEvent(monitor->ev);
	ReleaseMutex(monitor->mx);
}
