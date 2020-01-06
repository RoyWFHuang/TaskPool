#ifndef __task_action_hook_H__
#define __task_action_hook_H__

#include <stdint.h>
#include <unistd.h>

typedef struct _tTaskActionHook
{
    int (*initial)(void*);
    int (*start)(uint8_t cmd, int32_t len, uint8_t *msg);
    int (*free)(void*);
} tTaskActionHook;


#endif

