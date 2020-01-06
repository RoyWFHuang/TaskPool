#ifndef __task_pool_macro_H__
#define __task_pool_macro_H__



#ifdef CONSOLE_DEBUG
#   define UDT_DEBUG_PRINT(fmt, str...) \
        printf(\
        "%16.16s(%4d) - %16.16s: " fmt, \
        __FILE__, __LINE__, __func__, ##str)
#   define UDT_ERR_PRINT(fmt, str...) \
        printf(\
        "%16.16s(%4d) - %16.16s: **** " fmt, \
        __FILE__, __LINE__, __func__, ##str)
#else
#   include "define.h"
#   define UDT_DEBUG_PRINT PRINT_DBG
#   define UDT_ERR_PRINT PRINT_ERR
#endif


#define RAII_VARIABLE(vartype, varname, initval, dtor) \
    void _dtor_ ## varname (vartype * v) { dtor(*v); } \
    vartype varname __attribute__((cleanup(_dtor_ ## varname))) = (initval)


#endif