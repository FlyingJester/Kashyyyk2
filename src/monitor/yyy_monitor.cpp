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

#include "yyy_monitor.hpp"

#include "yyy_monitor_backend.h"

#include <stdlib.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*/

namespace YYY {

/*---------------------------------------------------------------------------*/

Monitor::Monitor()
  : m_monitor(static_cast<struct YYY_Monitor*>(malloc(YYY_MonitorSize()))){
    YYY_InitMonitor(m_monitor);
}

/*---------------------------------------------------------------------------*/

Monitor::~Monitor(){
    YYY_DestroyMonitor(m_monitor);
    free(m_monitor);
}

/*---------------------------------------------------------------------------*/
    
void Monitor::lock(){
    YYY_LockMonitor(m_monitor);
}

/*---------------------------------------------------------------------------*/

void Monitor::unlock(){
    YYY_UnlockMonitor(m_monitor);
}

/*---------------------------------------------------------------------------*/

void Monitor::wait(){
    YYY_WaitMonitor(m_monitor);
}

/*---------------------------------------------------------------------------*/

void Monitor::notify(){
    YYY_NotifyMonitor(m_monitor);
}

/*---------------------------------------------------------------------------*/

void Monitor::notifyAll(){
    YYY_NotifyAllMonitor(m_monitor);
}

} // namespace YYY
