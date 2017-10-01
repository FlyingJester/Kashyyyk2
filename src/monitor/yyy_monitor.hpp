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
