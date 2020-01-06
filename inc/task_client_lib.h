#ifndef __task_client_lib_H__
#define __task_client_lib_H__

#include <stdint.h>
#include <unistd.h>

int32_t send_msg(int32_t fd, int32_t uid,
    int32_t cmd, const uint8_t *msg, int32_t len,
    tPayLoadMsg *payload);

#endif
