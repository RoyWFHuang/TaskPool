#ifndef __ipc_ctrl_H__
#define __ipc_ctrl_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>

int listenIpcSocket(int socket_int);

int acceptIpcSocket(int socket_int);

int createAndBindIpcSocket(char *addr_pchar);

int createAndConnectIpcSocket(char *addr_pchar);


#endif

