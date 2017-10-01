#include "yyy_network.h"

#ifdef YYY_NETWORK_WIN32
    #include "yyy_network_win32.h"
#else
    #include "yyy_network_unix.h"
#endif
    
#include <poll.h>
#include <sys/ioctl.h>

#include <assert.h>
#include <stdlib.h>

struct YYY_SocketGroup{
    struct pollfd *fds;
    void **user_data;
    nfds_t num_fds, capacity;
    int pipe_fd[2];
};

/*****************************************************************************/

unsigned YYY_SocketGroupSize(){
    return sizeof(struct YYY_SocketGroup);
}

/*****************************************************************************/

void YYY_InitSocketGroup(struct YYY_SocketGroup *group){
    group->fds = NULL;
    group->num_fds = 0;
    pipe(group->pipe_fd);
}

/*****************************************************************************/

void YYY_DestroySocketGroup(struct YYY_SocketGroup *group){
    close(group->pipe_fd[0]);
    close(group->pipe_fd[1]);
    
    free(group->fds);
    free(group->user_data);
}

/*****************************************************************************/

void YYY_AddSocketToGroup(struct YYY_NetworkSocket *socket, void *user_data,
    struct YYY_SocketGroup *group){
    
    assert(group->capacity >= group->num_fds);
    if(group->capacity <= group->num_fds + 1){
        unsigned i = group->capacity;
        
        if(i == 0){
            group->capacity = 8;
            group->fds = calloc(sizeof(struct pollfd), 8);
            group->user_data = calloc(sizeof(void*), 8);
            group->fds->fd = group->pipe_fd[0];
            group->fds->events = POLLIN;
            i++;
        }
        else{
            group->capacity <<= 1;
            group->fds = realloc(group->fds, i * sizeof(struct pollfd) << 1);
            group->user_data = realloc(group->user_data, i * sizeof(void*) << 1);
        }
        
        do{
            group->fds[i].events = POLLIN;
            group->user_data[i] = NULL;
#ifndef NDEBUG
            group->fds[i].fd = -1;
#endif
        }while(++i < group->capacity);
    }
    
    {
        const nfds_t n = group->num_fds++;
        group->fds[n].fd = socket->socket;
        group->user_data[n] = user_data;
    }
}

/*****************************************************************************/

enum YYY_NetworkError YYY_RemoveSocketFromGroup(struct YYY_NetworkSocket *socket,
    struct YYY_SocketGroup *group, void **out_user_data){
    
    unsigned i = 1;
    const int s = socket->socket;
    while(i < group->num_fds){
        if(group->fds[i].fd == s){
            /* Swap the fd and user data from the last element in place of the
             * current element. This results in a single data move, instead of
             * shifting the entire array over. */
            const unsigned n = --group->num_fds;
            out_user_data[0] = group->user_data[i];
            if(i != n){
                group->fds[i].fd = group->fds[n].fd;
                group->user_data[i] = group->user_data[n];
            }
            return eSuccess;
        }
    }
    return eFailure;    
}

/*****************************************************************************/

void YYY_PokeSocketGroup(struct YYY_SocketGroup *group){
    char x = 0;
    write(group->pipe_fd[1], &x, 1);
}

/*****************************************************************************/

enum YYY_NetworkError YYY_WaitOnSocketGroup(struct YYY_SocketGroup *group,
    long micro_time,
    void **out_user_data){
    
    const nfds_t num_fds = group->num_fds;
    const int timeout =
        (micro_time < 0) ? -1 : 
        (micro_time == 0) ? 0 :
        ((micro_time / 1000) + 1);
    
    const int err = poll(group->fds, num_fds, timeout);
    
    if(err < 0)
        return eFailure;
    if(err == 0)
        return eTimeout;
    {
        unsigned i = 1;
        if(group->fds->revents & POLLIN)
            return ePoked;
        
        while(i < num_fds){
            if(group->fds[i].revents & POLLIN){
                out_user_data[0] = group->user_data[i];
                return eSuccess;
            }
            else if(group->fds[i].revents & (POLLHUP | POLLERR)){
                out_user_data[0] = group->user_data[i];
                return eDisconnected;
            }
            i++;
        }
    }
    return eFatal;
}
