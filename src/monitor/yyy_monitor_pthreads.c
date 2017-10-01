#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

struct YYY_Monitor {
	pthread_cond_t cv;
	pthread_mutex_t mutex;
};

unsigned YYY_MonitorSize(){
    return sizeof(struct YYY_Monitor);
}

void YYY_InitMonitor(struct YYY_Monitor *monitor){
    int err;
    
    pthread_condattr_t cv_attr;
    pthread_mutexattr_t mx_attr;

    err = pthread_condattr_init(&cv_attr);
    err |= pthread_mutexattr_init(&mx_attr);
    assert(err==0);

    err = pthread_cond_init(&(monitor->cv), &cv_attr);
    err |= pthread_mutex_init(&(monitor->mutex), &mx_attr);
    assert(err==0);

    err = pthread_condattr_destroy(&cv_attr);
    err |= pthread_mutexattr_destroy(&mx_attr);
    assert(err==0);
}

void YYY_DestroyMonitor(struct YYY_Monitor *monitor){
    int err;

    err = pthread_cond_destroy(&(monitor->cv));
    err |= pthread_mutex_destroy(&(monitor->mutex));
    assert(err==0);
}

void YYY_LockMonitor(struct YYY_Monitor *monitor){
    pthread_mutex_lock(&(monitor->mutex));
}

void YYY_UnlockMonitor(struct YYY_Monitor *monitor){
    pthread_mutex_unlock(&(monitor->mutex));
}

void YYY_WaitMonitor(struct YYY_Monitor *monitor){
    const int err = pthread_cond_wait(&(monitor->cv), &(monitor->mutex));
    (void)err;
    assert(err==0);
}

void YYY_NotifyMonitor(struct YYY_Monitor *monitor){
    const int err = pthread_cond_signal(&(monitor->cv));
    (void)err;
    assert(err==0);
}

void YYY_NotifyAllMonitor(struct YYY_Monitor *monitor){
    const int err = pthread_cond_broadcast(&(monitor->cv));
    (void)err;
    assert(err==0);
}
