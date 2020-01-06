#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "ipc_ctrl.h"
#include "task_pool.h"
#include "task_pool_macro.h"
#include "macro.h"
#include "lib_method.h"
#include "errorno.h"

int32_t send_msg(int32_t fd, int32_t uid,
    int32_t cmd, const uint8_t *msg, int32_t len,
    tPayLoadMsg *payload)
{
    int32_t send_len = 0;

    if(NULL == msg)
        send_len = 1+4+4;
    else
        send_len = len+1+4+4;

    RAII_VARIABLE(uint8_t *, sendbuff, NULL, free);
    RAII_VARIABLE(uint8_t *, receive_data, NULL, free);
    //check_null_input(payload);

    sendbuff = calloc((send_len), sizeof(uint8_t));

    uint8_t *ptr = sendbuff;
    int32_t tmp_32;
    uint8_t tmp_u8;
    tmp_u8 = cmd;
    memcpy(ptr, &tmp_u8, sizeof(uint8_t));
    ptr+=1;

    tmp_32 = uid;
    memcpy(ptr, &tmp_32, sizeof(int32_t));
    ptr+=4;

    tmp_32 = len;
    memcpy(ptr, &tmp_32, sizeof(int32_t));
    ptr+=4;

    if(NULL != msg && 0 != len)
        memcpy(ptr, msg, len*sizeof(uint8_t));
    ssize_t size = write(fd, sendbuff, send_len);
    if(-1 == size)
    {
        UDT_ERR_PRINT("write data fail");
        return ERROR_CODE_NONEXPECT_ERROR;
    }


    if(NULL != payload)
    {
        receive_data = calloc(128, sizeof(uint8_t));
        int32_t ret = read(fd, receive_data, 128);

        if(ret < 0)
        {
            UDT_ERR_PRINT("read data fail");
            return ERROR_CODE_NONEXPECT_ERROR;
        }
        memset(payload, 0, sizeof(tPayLoadMsg));
        parse_payload(receive_data, payload);
        if(cmd == payload->cmd)
            return ERROR_CODE_SUCCESS;
        else
            return ERROR_CODE_NONEXPECT_ERROR;
    }
    return ERROR_CODE_SUCCESS;
}