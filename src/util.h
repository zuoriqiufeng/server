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


namespace dx {

/**
 * @brief 
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




}

#endif
