#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

#define max(x,y) ((x > y) ? x : y)

int
main(void) {
    int pipes[2];
    int res = pipe(pipes);
    if (res != 0) abort();
    int r = pipes[0];
    int w = pipes[1];
    res = close(w);
    if (res != 0) abort();
    fd_set readfds; FD_ZERO(&readfds);
    fd_set writefds; FD_ZERO(&writefds);
    fd_set exceptfds; FD_ZERO(&exceptfds);
    //struct timeval *timeout = NULL;
    //FD_SET(r, &readfds);
    FD_SET(w, &writefds);
    res = select(max(r,w)+1, &readfds, &writefds, &exceptfds, NULL);

    return 0;
}

