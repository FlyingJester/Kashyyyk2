#include "yyy_thread.h"

#include "kernel/OS.h"

struct YYY_Thread {
    thread_id m_thread;
    void *m_arg, *m_value;
    yyy_thread_function_t m_callback;
};

static int32 yyy_thread_wrapper(void *data){
    YYY_Thread *const thr = static_cast<YYY_Thread*>(data);
    thr->m_value = thr->callback(thr->arg);
    return 0;
}

unsigned YYY_ThreadSize(){
    return sizeof(YYY_Thread);
}

void YYY_InitThread(YYY_Thread *thr, yyy_thread_function_t func){
    thr->m_callback = func;
    thr->m_thread =
        spawn_thread(yyy_thread_wrapper, "yyy_worker", B_NORMAL_PRIORITY, thr);
}

void YYY_StartThread(YYY_Thread *thr, void *data){
    thr->m_arg = data;
    thr->m_value = NULL;
    resume_thread(thr->m_thread);
}

void *YYY_JoinThread(YYY_Thread *thr){
    status_t i;
    wait_for_thread(thr->m_thread, &i);
    void *const value = thr->value;
    thr->m_value = NULL;
    return value;
}

void YYY_DestroyThread(YYY_Thread *thr){
    (void)thr;
}
