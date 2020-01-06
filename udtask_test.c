
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include "ipc_ctrl.h"
#include "task_pool.h"
#include "task_client_lib.h"



int main()
{
    tPayLoadMsg payload;
    memset(&payload, 0, sizeof(tPayLoadMsg));
    int fd_int = createAndConnectIpcSocket(ULDL_SOCKET);
    int ret = send_msg(
        fd_int, -1, eULTASK, "test ul msg", strlen("test ul msg"), &payload);
    close(fd_int);
    if(0 != ret)
    {
        printf("error\n");
        exit(0);
    }
    int32_t uid = payload.uid;
    free_tPayLoadMsg(&payload);
    sleep(5);
    fd_int = createAndConnectIpcSocket(ULDL_SOCKET);
    ret = send_msg(fd_int, uid, eCANCELTASK, NULL, 0, NULL);
    if(0 != ret)
    {
        printf("error\n");
        exit(0);
    }

    sleep(5);
    fd_int = createAndConnectIpcSocket(ULDL_SOCKET);
    ret = send_msg(fd_int, -1, eExit, NULL, 0, NULL);
    if(0 != ret)
    {
        printf("error\n");
        exit(0);
    }
    //sleep(1);
    //size = write(fd_int, sendbuff, 18);
    //sleep(1);
    close(fd_int);
    return 0;
}