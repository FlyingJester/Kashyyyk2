#pragma once
#ifndef YYY_THREAD_HEADER_
#define YYY_THREAD_HEADER_

#ifdef __cplusplus
extern "C" {
#endif

struct YYY_Thread;
unsigned YYY_ThreadSize();
typedef void *(*yyy_thread_function_t)(void*);
void YYY_InitThread(struct YYY_Thread *thr, yyy_thread_function_t func);
void YYY_StartThread(struct YYY_Thread *thr, void *data);
void *YYY_JoinThread(struct YYY_Thread *thr);
void YYY_DestroyThread(struct YYY_Thread *thr);

#ifdef __cplusplus
}
#endif

#endif /* YYY_THREAD_HEADER_ */
