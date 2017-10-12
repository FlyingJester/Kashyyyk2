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

#ifndef YYY_MONITOR_HPP
#define YYY_MONITOR_HPP
#pragma once

#ifdef __WATCOMC__
// Disable the tautological conditional warnings and the unreachable code
// warnings. These are triggered by branching on template parameters in the
// MonitorLocker, and are expected.
#pragma warning 368 10
#pragma warning 13 10
#endif

struct YYY_Monitor;

namespace YYY {

class Monitor {
public:
    Monitor();
    ~Monitor();
    
    void lock();
    void unlock();
    
    // The monitor must be locked for wait to be called.
    void wait();
    
    // The monitor must be unlocked before notify is called
    void notify();
    void notifyAll();

private:
    struct YYY_Monitor *const m_monitor;
};

enum EnumMonitorLockerAction {
    eUnlock,
    eNotify,
    eNotifyAll
};

template<EnumMonitorLockerAction Action>
struct MonitorLocker {
private:
    Monitor &m_monitor;
public:

    MonitorLocker(Monitor &monitor)
      : m_monitor(monitor){
        m_monitor.lock();
    }
    
    ~MonitorLocker(){
        m_monitor.unlock();
        
        if(Action==eNotify)
            m_monitor.notify();
        else if(Action==eNotifyAll)
            m_monitor.notifyAll();
    }

};

typedef MonitorLocker<eUnlock> Locker;
typedef MonitorLocker<eNotify> Notifier;
typedef MonitorLocker<eNotifyAll> Broadcaster;

#ifdef __WATCOMC__
#pragma warning 368 9
#pragma warning 13 9
#endif

} //namespace YYY

#endif // YYY_MONITOR_HPP
