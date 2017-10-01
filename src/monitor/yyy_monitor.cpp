#include "yyy_monitor.hpp"

#include "yyy_monitor_backend.h"

#include <stdlib.h>
#include <stdio.h>

namespace YYY {

Monitor::Monitor()
  : m_monitor(static_cast<struct YYY_Monitor*>(malloc(YYY_MonitorSize()))){
    YYY_InitMonitor(m_monitor);
}

Monitor::~Monitor(){
    YYY_DestroyMonitor(m_monitor);
    free(m_monitor);
}
    
void Monitor::lock(){
    YYY_LockMonitor(m_monitor);
}

void Monitor::unlock(){
    YYY_UnlockMonitor(m_monitor);
}

void Monitor::wait(){
    YYY_WaitMonitor(m_monitor);
}

void Monitor::notify(){
    YYY_NotifyMonitor(m_monitor);
}
void Monitor::notifyAll(){
    YYY_NotifyAllMonitor(m_monitor);
}

} // namespace YYY
