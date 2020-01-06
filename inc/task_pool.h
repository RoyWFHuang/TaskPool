#ifndef __task_pool_H__
#define __task_pool_H__

#include <stdint.h>
#include <unistd.h>

#define ULDL_SOCKET "./uldltask"
enum {
    eINIT,
    eULTASK,
    eDLTASK,
    eCANCELTASK,
    eExit,
};

typedef struct _tPayLoadMsg
{
    uint8_t cmd; // for enum
    int32_t uid;
    int32_t len;
    uint8_t *content;
}tPayLoadMsg;


int32_t free_tPayLoadMsg(tPayLoadMsg *ptr);

int32_t parse_payload(const uint8_t *bytedata, tPayLoadMsg *payload);

void *clean_table_thread(void *arg);

int32_t read_data(int32_t fd);

#endif
