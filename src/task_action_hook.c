
#include "task_action_hook.h"

static int initial_task(void * argv)
{
    return 0;
}

static int start_task(uint8_t cmd, int32_t len, uint8_t *msg)
{
    while(1);
    return 0;
}

static int free_task(void * argv)
{
    return 0;
}

tTaskActionHook __g_action_task=
{
    .initial = &initial_task,
    .start = &start_task,
    .free = &free_task,
};
