
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>


#include "task_pool_macro.h"
#include "proc_table.h"
#include "task_pool.h"
#include "errorno.h"
#include "macro.h"

#define MAX_RECO_SIZE 512

static tProcTable __g_proc_table[MAX_RECO_SIZE];

int32_t set_proctable()
{
    for(int32_t i =0; i < MAX_RECO_SIZE; i++)
    {
        int8_t test = __sync_fetch_and_or(&__g_proc_table[i].mask, 0x01);
        if(!test)
            return i;
    }
    return -1;
}

int32_t unset_proctable_byuid(int32_t uid)
{
    __g_proc_table[uid].pid = 0;
    free_to_NULL(__g_proc_table[uid].msg);
    __sync_fetch_and_and(&__g_proc_table[uid].mask, 0x00);
    return 0;
}

int32_t set_table_info(
    int32_t index, int32_t len, const uint8_t* content, pid_t child_pid)
{
    __g_proc_table[index].pid = (int64_t)child_pid;
    __g_proc_table[index].msg = calloc(len, sizeof(char));
    memcpy(__g_proc_table[index].msg, content,
        sizeof(char)*len);
    return 0;
}

pid_t get_pid_proctable(int32_t uid)
{
    return __g_proc_table[uid].pid;
}

int32_t unset_proctable_bypid(const pid_t pid)
{
    for(int32_t i =0; i < MAX_RECO_SIZE; i++)
    {
        if(pid == __g_proc_table[i].pid)
        {
            __g_proc_table[i].pid = 0;
            free_to_NULL(__g_proc_table[i].msg);
            __sync_fetch_and_and(&__g_proc_table[i].mask, 0x00);
            return ERROR_CODE_SUCCESS;
        }
    }
    return 0;
}

void print_proctable()
{
    UDT_DEBUG_PRINT("start print proc table\n");
    for(int32_t i =0; i < MAX_RECO_SIZE; i++)
    {
        if(__g_proc_table[i].mask != 0)
        {
            UDT_DEBUG_PRINT("| pid[%04d][%s] | \n",
                __g_proc_table[i].pid,
                __g_proc_table[i].msg);
        }
    }
    UDT_DEBUG_PRINT("end start print proc table\n");
}

int32_t init_proctable()
{
    memset(&__g_proc_table, 0, sizeof(tProcTable));
    return ERROR_CODE_SUCCESS;
}