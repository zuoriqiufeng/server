/**
 * @file util.h
 * @author Dingx (dingx@info2soft.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __DX_UTIL_H__
#define __DX_UTIL_H__
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

namespace dx {

/**
 * @brief 返回线程Id
 * 
 * @return 
 */
pid_t GetThreadId();

/**
 * @brief Get the Fiber Id object
 * 
 * @return uint32_t 
 */
uint32_t GetFiberId();


/**
 * @brief 打印堆栈
 * 
 * @param  bt 栈数组
 * @param  size 栈层数
 * @param  skip 跳过前面几层
 */
void Backtrace(std::vector<std::string>& bt, int size, int skip = 1);
std::string BacktraceToString(int size, int skip = 2, const std::string& prefix = "");

}

#endif
