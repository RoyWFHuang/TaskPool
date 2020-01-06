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

const char service_interp[] __attribute__((section(".interp"))) = "/lib/ld-linux.so.2";

uint8_t __g_exit_flag = 0;

#ifndef autofree_tPayLoadMsg
#define autofree_tPayLoadMsg \
    __attribute__((cleanup(free_tPayLoadMsg)))
#endif

extern tTaskActionHook __g_action_task;
/**
 * message format: | 1*8 | 4*8 |4*8 | n*8     |
 *                 | CMD | UID |LEN | MESSAGE |
 **/


int32_t free_tPayLoadMsg(tPayLoadMsg *ptr)
{
    if(!ptr) return ERROR_CODE_SUCCESS;
    free_to_NULL(ptr->content);
    return ERROR_CODE_SUCCESS;
}

int32_t parse_payload(const uint8_t *bytedata, tPayLoadMsg *payload)
{
//    int32_t msglen_int = 0;
//    int32_t numof_msg = 0;
    uint8_t *tmp_data = NULL;
    check_null_input(payload);
    tmp_data = (uint8_t *)bytedata;

    memcpy(&payload->cmd, tmp_data, sizeof(uint8_t));
    tmp_data+=1;
    memcpy(&payload->uid, tmp_data, sizeof(int32_t));
    tmp_data+=4;
    memcpy(&payload->len, tmp_data, sizeof(int32_t));
    tmp_data+=4;
    if(payload->len > 0)
    {
        payload->content = calloc(payload->len, sizeof(uint8_t));
        memcpy(payload->content, tmp_data, (payload->len)*sizeof(uint8_t));
        tmp_data+=payload->len;
    }
#ifdef UDT_DEBUG_MODE
    UDT_DEBUG_PRINT("cmd = %d\n", payload->cmd );
    UDT_DEBUG_PRINT("uid = %d\n", payload->uid );
    UDT_DEBUG_PRINT("len = %d\n", payload->len );
    UDT_DEBUG_PRINT("content = %s\n", payload->content );
#endif
    return ERROR_CODE_SUCCESS;
}

void *clean_table_thread(void *arg)
{
    pid_t wpid;
    int32_t status = 0;
#ifdef UDT_DEBUG_MODE
    UDT_DEBUG_PRINT("start thread\n");
#endif
    while(likely(!__g_exit_flag))
    {
        while ((wpid = wait(&status)) > 0)
        {
#ifdef UDT_DEBUG_MODE
            UDT_DEBUG_PRINT("Exit status of %d was %d (%s)\n", (int)wpid, status,
                (status > 0) ? "error currect" : "success done");
            print_proctable();
#endif
            unset_proctable_bypid(wpid);
#ifdef UDT_DEBUG_MODE
            print_proctable();
#endif
        }
    }
#ifdef UDT_DEBUG_MODE
    UDT_DEBUG_PRINT("exit thread\n");
    UDT_DEBUG_PRINT("check proctable is clean");
    print_proctable();
#endif
    pthread_exit(NULL);
}

int32_t __kill_task(pid_t index)
{
    return 0;
}

int32_t read_data(int32_t fd)
{
    int exitflag = 0;
    UDT_DEBUG_PRINT("fd = %d\n", fd );
    autofree_tPayLoadMsg tPayLoadMsg payload;
    memset(&payload, 0, sizeof(tPayLoadMsg));

    uint8_t *receive_data = NULL;
    receive_data = calloc(1024, sizeof(uint8_t));
    int32_t len = read(fd, receive_data, 1024);
    if(len < 0)
    {
        UDT_ERR_PRINT("read data fail");
        free(receive_data);
        return 0;
    }
    int32_t ret =
        parse_payload((const uint8_t *)receive_data, &payload);
    free(receive_data);

    if(ERROR_CODE_SUCCESS != ret)
    {
        UDT_ERR_PRINT("parser data fail");
        return 0;
    }
    //set_proc_table(cmd_uint8, getpid(), action_data);


    pid_t child_pid ;

    switch(payload.cmd)
    {
        case eINIT:
            break;
        case eULTASK:
        case eDLTASK:
        {
            int32_t uid = set_proctable();
            child_pid  = fork();
            switch(child_pid)
            {
                case -1:
                    UDT_ERR_PRINT("fork fail");
                    unset_proctable_byuid(uid);
                    exit(-1);
                    break;
                case 0: // child proc
#ifdef UDT_DEBUG_MODE
                    UDT_DEBUG_PRINT("Child's PID is %d\n", getpid());
#endif
                    __g_action_task.start(payload.cmd,
                        payload.len,
                        payload.content);
#ifdef UDT_DEBUG_MODE
                    UDT_DEBUG_PRINT("end of Child's PID is %d\n", getpid());
#endif
                    //close(fd);
                    //exit(1); // wait return state 256
                    exit(0); // wait return state 0
                    break;
                default: //parent proc
#ifdef UDT_DEBUG_MODE
                    UDT_DEBUG_PRINT("Parent's PID is %d\n", getpid());
#endif
                    set_table_info(uid, payload.len,
                        (const uint8_t *)payload.content, child_pid);

                    send_msg(fd, uid, payload.cmd, NULL, 0, NULL);
                    //uint8_t *sendbuff = (uint8_t *)&uid;
                    //ssize_t size = write(fd, sendbuff, 4);
                    break;
            }
        }
            break;
        case eCANCELTASK:
            {
                pid_t chpid = get_pid_proctable(payload.uid);
                kill(chpid, SIGKILL);
                //unset_proctable_bypid(chpid);
            }
            break;
        case eExit:
            close(fd);
            exitflag = 1;
            break;
        default:
            UDT_ERR_PRINT("ERROR format\n");
            break;
    }
//        UDT_DEBUG_PRINT("in this pid is %d\n", getpid());
/*
    if(child_pid)
    {
        pid_t wpid;
        int32_t status = 0;
        UDT_DEBUG_PRINT("in this pid is %d\n", getpid());
        while ((wpid = wait(&status)) > 0)
        {
            UDT_DEBUG_PRINT("Exit status of %d was %d (%s)\n", (int)wpid, status,
                (status > 0) ? "accept" : "reject");
            unset_proctable_bypid(wpid);
        }
    }
*/
    return exitflag;
}
