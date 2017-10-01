#include "yyy_thread.h"

#include <cthreads.h>

struct YYY_Thread{
    void *arg;
    yyy_thread_function_t func;
    cthread_t thread;
}

static any_t yyy_thread_wrapper(any_t arg){
    struct YYY_Thread *const thr = (struct YYY_Thread*)thr;
    return thr->func(thr->arg);
}

unsigned YYY_ThreadSize(){
    return sizeof(struct YYY_Thread);
}

void YYY_InitThread(struct YYY_Thread *thr, yyy_thread_function_t func){
    thr->func = func;
}

void YYY_StartThread(struct YYY_Thread *thr, void *data){
    thr->arg = data;
    thr->thread = cthread_fork(yyy_thread_wrapper, thr);
}

void *YYY_JoinThread(struct YYY_Thread *thr){
    return cthread_join(thr->thread);
}

void YYY_DestroyThread(struct YYY_Thread *thr){

}
