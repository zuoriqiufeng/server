/**
 * @file scheduler.cpp
 * @author Dingx (dingx@info2soft.com)
 * @brief 
 * 
 * @version 0.1
 * @date 2024-09-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "scheduler.h"
#include "log.h"


namespace dx {

Logger::ptr g_logger = SERVER_LOG_NAME("system");

Scheduler::Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name) {

}

Scheduler::~Scheduler() {

}

Scheduler* Scheduler::GetThis() {

}

Fiber* Scheduler::GetMainFiber() {

}

void Scheduler::Start() {

}

void Scheduler::Stop() {

}

void Scheduler::Tickle() {

}


}
