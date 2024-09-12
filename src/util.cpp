/**
 * @file util.cpp
 * @author Dingx (dingx@info2soft.com)
 * @brief 
 * 
 * @version 0.1
 * @date 2024-09-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "util.h"
#include <syscall.h>
#include <csignal>
#include <iostream>
#include <zconf.h>
namespace dx {
pid_t GetThreadId() {
    return (pid_t)syscall(SYS_gettid);
}

uint32_t GetFiberId() {

    return 0;
}

}
