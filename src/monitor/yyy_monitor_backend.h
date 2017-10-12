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

#ifndef YYY_MONITOR_BACKEND_H
#define YYY_MONITOR_BACKEND_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

struct YYY_Monitor;

/*---------------------------------------------------------------------------*/

#ifdef __GNUC__
__attribute__((const))
#endif
unsigned YYY_MonitorSize();

/*---------------------------------------------------------------------------*/

void YYY_InitMonitor(struct YYY_Monitor *monitor);

/*---------------------------------------------------------------------------*/

void YYY_DestroyMonitor(struct YYY_Monitor *monitor);

/*---------------------------------------------------------------------------*/

void YYY_LockMonitor(struct YYY_Monitor *monitor);

/*---------------------------------------------------------------------------*/

void YYY_UnlockMonitor(struct YYY_Monitor *monitor);

/*---------------------------------------------------------------------------*/

void YYY_WaitMonitor(struct YYY_Monitor *monitor);

/*---------------------------------------------------------------------------*/

void YYY_NotifyMonitor(struct YYY_Monitor *monitor);

/*---------------------------------------------------------------------------*/

void YYY_NotifyAllMonitor(struct YYY_Monitor *monitor);

#ifdef __cplusplus
}
#endif

#endif /* YYY_MONITOR_BACKEND_H */
