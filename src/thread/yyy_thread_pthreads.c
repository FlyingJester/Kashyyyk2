#include "yyy_thread.h"

#include <pthread.h>

struct YYY_Thread{
    yyy_thread_function_t func;
    pthread_t thread;
};

unsigned YYY_ThreadSize(){
    return sizeof(struct YYY_Thread);
}

void YYY_InitThread(struct YYY_Thread *thr, yyy_thread_function_t func){
    thr->func = func;
}

void YYY_StartThread(struct YYY_Thread *thr, void *data){
    pthread_create(&thr->thread, NULL, thr->func, data);
}

void *YYY_JoinThread(struct YYY_Thread *thr){
    void *r;
    pthread_join(thr->thread, &r);
    return r;
}

void YYY_DestroyThread(struct YYY_Thread *thr){
    (void)thr;
}
