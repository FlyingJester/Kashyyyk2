#include "yyy_monitor_backend.h"

#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include <Windows.h>
#include <assert.h>
#undef WIN32_LEAN_AND_MEAN

struct YYY_Monitor {
	CONDITION_VARIABLE cv;
	CRITICAL_SECTION cs;
};

unsigned YYY_MonitorSize(){
    return sizeof(struct YYY_Monitor);
}

void YYY_InitMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
	InitializeConditionVariable(&(monitor->cv));
	InitializeCriticalSection(&(monitor->cs));
}

void YYY_DestroyMonitor(struct YYY_Monitor *monitor){
	assert(monitor);
	
    WakeAllConditionVariable(&(monitor->cv));
	DeleteCriticalSection(&(monitor->cs));
}

void YYY_LockMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
	EnterCriticalSection(&(monitor->cs));
}

void YYY_UnlockMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
	LeaveCriticalSection(&(monitor->cs));
}

void YYY_WaitMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
	SleepConditionVariableCS(&(monitor->cv), &(monitor->cs), INFINITE);
}

void YYY_NotifyMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    WakeConditionVariable(&(monitor->cv));
}

void YYY_NotifyAllMonitor(struct YYY_Monitor *monitor){
    assert(monitor != NULL);
    WakeAllConditionVariable(&(monitor->cv));
}
