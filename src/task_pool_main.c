#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "define.h"
#include "ipc_ctrl.h"
#include "proc_table.h"
#include "task_pool.h"
#include "task_action_hook.h"
#include "task_client_lib.h"
#include "task_pool_macro.h"
#include "macro.h"
#include "errorno.h"

extern uint8_t __g_exit_flag;

int main()
{
    //initialEpollDeviceEvent();
    init_proctable();

    pthread_t cleantable;
    pthread_create(&cleantable, NULL, clean_table_thread, NULL);

    uint32_t fd = createAndBindIpcSocket(ULDL_SOCKET);
    if(-1 == fd) return -1;
    listenIpcSocket(fd);
    while(likely(!__g_exit_flag))
    {
#ifdef UDT_DEBUG_MODE
        UDT_DEBUG_PRINT("wait for socket\n");
#endif
        uint32_t accfd = acceptIpcSocket(fd);
#ifdef UDT_DEBUG_MODE
        UDT_DEBUG_PRINT("accfd_int = %d\n", accfd);
#endif
        __g_exit_flag = read_data(accfd);
    }
    pthread_join(cleantable, NULL);
    return 0;
}

