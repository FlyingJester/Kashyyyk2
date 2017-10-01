#include "yyy_thread.h"

#include <Windows.h>

struct YYY_Thread{
    void *value, *arg;
    yyy_thread_function_t func;
    HANDLE thread;
};

static DWORD WINAPI yyy_thread_wrapper(LPVOID arg){
    struct YYY_Thread *const thr = arg;
    thr->value = thr->func(thr->arg);
    return 0;
}

unsigned YYY_ThreadSize(){
    return sizeof(struct YYY_Thread);
}

void YYY_InitThread(struct YYY_Thread *thr, yyy_thread_function_t func){
    thr->func = func;
    thr->thread = CreateThread(NULL, 0, yyy_thread_wrapper, thr, CREATE_SUSPENDED, NULL);
}

void YYY_StartThread(struct YYY_Thread *thr, void *data){
    thr->arg = data;
    ResumeThread(thr->thread);
}

void *YYY_JoinThread(struct YYY_Thread *thr){
    WaitForSingleObject(thr->thread, INFINITE);
    return thr->value;
}

void YYY_DestroyThread(struct YYY_Thread *thr){
    CloseHandle(thr->thread);
}
