/**
 * @file macro.h 定义Assert 顺便打印堆栈
 * @author Dingx (dingx@info2soft.com)
 * @brief 
 * 
 * @version 0.1
 * @date 2024-09-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __SERVER_MACRO_H__
#define __SERVER_MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"


#define SERVER_ASSERT(x) \
    if(!(x)) { \
        SERVER_LOG_ERROR(SERVER_LOG_ROOT()) << "ASSERTIONG: "  #x \
            << "\nbacktrace:\n" \
            << dx::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define SERVER_ASSERT_ARG(x, w) \
    if(!(x)) { \
        SERVER_LOG_ERROR(SERVER_LOG_ROOT()) << "ASSERTIONG: "  #x \
            << "\n" << #w \
            << "\nbacktrace:\n" \
            << dx::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }


#endif
