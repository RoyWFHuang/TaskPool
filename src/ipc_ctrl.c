#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include "ipc_ctrl.h"
#include "task_pool.h"

int listenIpcSocket(int socket_int)
{
	if (listen(socket_int, 1) < 0)
	{
		//UDT_ERR_PRINT("listen error\n");
		return -1;
	}
	return 1;
}

int acceptIpcSocket(int socket_int)
{
	struct sockaddr_un from_struct;
	int fromlen_int = 0,acceptsk_int = 0;
	memset (&from_struct,0,sizeof(struct sockaddr_un));
	acceptsk_int = accept(socket_int, (struct sockaddr *)&from_struct, (socklen_t*)&fromlen_int);
	if (acceptsk_int <= 0) {
		//TODO ... error handling
		//UDT_ERR_PRINT("server: accept error  = %d\n",acceptsk_int);
	}
	return acceptsk_int;
}

int createAndBindIpcSocket(char *addr_pchar)
{
	int socket_int, size_int;
	struct sockaddr_un un_struct;
	unlink(addr_pchar);
	memset(&un_struct, 0, sizeof(un_struct));
	un_struct.sun_family = AF_UNIX;

	strcpy(un_struct.sun_path, addr_pchar);
	if ((socket_int = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		//TODO ... error handling
		return -1;
	}
	size_int = sizeof(un_struct.sun_family) + strlen(un_struct.sun_path);
	if (bind(socket_int, (struct sockaddr *)&un_struct, size_int) < 0)
	{
		//TODO ... error handling
		close(socket_int);
		return -1;
	}


	return socket_int;
}

int createAndConnectIpcSocket(char *addr_pchar)
{
	int  socket_int;
	int size_int;

	struct sockaddr_un un_struct;
	memset(&un_struct, 0, sizeof(struct sockaddr_un));

	if ((socket_int = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0)
	{
		//TODO ... error handling
		return -1;
	}
	un_struct.sun_family = AF_UNIX;
	strcpy(un_struct.sun_path, addr_pchar);


	size_int = sizeof(un_struct.sun_family) + strlen(un_struct.sun_path);
	int ret_int = connect(socket_int,(struct sockaddr*)&un_struct, size_int) ;
	if ( ret_int < 0)
	{
		//TODO ... error handling
		return -1;
	}
	return socket_int;
}


