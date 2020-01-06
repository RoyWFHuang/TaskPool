#ifndef __proc_table_H__
#define __proc_table_H__

#include <stdint.h>
#include <unistd.h>


typedef struct _tProcTable
{
    // | 7 - 1    |  0   |
    // | reserved | used |
    uint8_t mask;
    uint8_t *msg;
    pid_t pid;
}tProcTable;

int32_t set_proctable();

int32_t unset_proctable_byuid(int32_t uid);

int32_t set_table_info(
    int32_t index, int32_t len, const uint8_t* content, pid_t child_pid);

pid_t get_pid_proctable(int32_t uid);

int32_t unset_proctable_bypid(int32_t uid);

void print_proctable();

int32_t init_proctable();

#endif

