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
#include <vector>
#include <execinfo.h>
#include "log.h"
#include "macro.h"
#include "fiber.h"

dx::Logger::ptr g_logger = SERVER_LOG_NAME("system");

namespace dx {

pid_t GetThreadId() {
    return (pid_t)syscall(SYS_gettid);
}

uint64_t GetFiberId() {
    return dx::Fiber::GetFiberId();
  
}

void Backtrace(std::vector<std::string>& bt, int size, int skip) {
    void** array = (void**)malloc(sizeof(void*) *size);
    size_t s = ::backtrace(array, size);
    
    char** strings = backtrace_symbols(array, s);
    if(strings == NULL) {
        SERVER_LOG_ERROR(g_logger) << "backtrace_symbols error";
    }

    for(size_t i = skip; i < s ; i++) {
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);
}


std::string BacktraceToString(int size, int skip, const std::string& prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); i++) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}


}
