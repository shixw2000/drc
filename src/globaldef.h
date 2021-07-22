/********************************************************************************

 **** Copyright (C), 2021,                                               ****

 ********************************************************************************
 * File Name     : globaldef.h
 * Author        : shixw
 * Date          : 2021-04-19
 * Description   : .C file function description
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2021-04-19
 *   Author      : shixw
 *   Modification: Created file

*************************************************************************************************************/
#ifndef __GLOBALDEF_H__
#define __GLOBALDEF_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>


#define OPC_API __attribute__((visibility("default")))

#define LOG_INFO(format,arg...) do {\
    fprintf(stdout, "<%s:%d>", __FILE__, __LINE__); \
    fprintf(stdout, format, ##arg); \
    fprintf(stdout, "\n"); \
} while(false)

#define BEG_NS(ns) namespace opc { namespace ns {
#define END_NS(ns) }}
#define USE_NS(ns) using namespace opc::ns
#define NS(ns) opc::ns

#define STD_NS(sym) std::sym
#define GLOBAL_NS(sym) ::sym

#define ATOMIC_INC(ptr) __sync_add_and_fetch((ptr), 1)

#define STR(x) #x
#define ERR_MSG strerror(errno)
#define CHK_NULL(ptr,info) do { \
    if(NULL == (ptr)) { \
        LOG_INFO("NULL_PTR| info=%s| err=%s|", (info), ERR_MSG); \
        return -1; \
    }\
} while (false)

#define CHK_EQ(x,expect,info) do { \
    if((x) != (expect)) { \
        LOG_INFO("CHK_EQ| x=%d| expect=%d| err=%s|", (x),(expect), (info), ERR_MSG); \
        return -1; \
    }\
} while (false)


static const int NODE_TREE_CHILD_NUM = 4;
static const int BOUNDARY_DIR_NUM = 4;
static const int AREA_CONTAIN_MIN_POLYGON = 1;
static const int AREA_MAX_DELIMIT_TIMES = 10;
static const double AREA_DELIMIT_MIN_WIDTH = 10.;
static const int AREA_MIN_WIDTH_TIMES_DISTANCE = 2;

#endif

