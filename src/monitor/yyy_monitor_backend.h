#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct YYY_Monitor;
unsigned YYY_MonitorSize();

void YYY_InitMonitor(struct YYY_Monitor *monitor);
void YYY_DestroyMonitor(struct YYY_Monitor *monitor);

void YYY_LockMonitor(struct YYY_Monitor *monitor);
void YYY_UnlockMonitor(struct YYY_Monitor *monitor);
void YYY_WaitMonitor(struct YYY_Monitor *monitor);
void YYY_NotifyMonitor(struct YYY_Monitor *monitor);
void YYY_NotifyAllMonitor(struct YYY_Monitor *monitor);

#ifdef __cplusplus
}
#endif
