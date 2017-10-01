#include "yyy_monitor_backend.h"

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x400
#include <stdlib.h>
#include <Windows.h>
#include <assert.h>
#undef WIN32_LEAN_AND_MEAN

struct YYY_Monitor {
	HANDLE ev;
	HANDLE mx;
};

unsigned YYY_MonitorSize(){
    return sizeof(struct YYY_Monitor);
}

void YYY_InitMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    monitor->ev = CreateEvent(NULL, FALSE, FALSE, NULL);
	monitor->mx = CreateMutex(NULL, FALSE, NULL);
}

void YYY_DestroyMonitor(struct YYY_Monitor *monitor){
	assert(monitor);
    CloseHandle(monitor->ev);
    CloseHandle(monitor->mx);
}

void YYY_LockMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    WaitForSingleObject(monitor->mx, INFINITE);
}

void YYY_UnlockMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
	ReleaseMutex(monitor->mx);
}

void YYY_WaitMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
	ReleaseMutex(monitor->mx);
    WaitForSingleObject(monitor->ev, INFINITE);
}

void YYY_NotifyMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    WaitForSingleObject(monitor->mx, INFINITE);
    SetEvent(monitor->ev);
	ReleaseMutex(monitor->mx);
}

void YYY_NotifyAllMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    WaitForSingleObject(monitor->mx, INFINITE);
    PulseEvent(monitor->ev);
	ReleaseMutex(monitor->mx);
}
