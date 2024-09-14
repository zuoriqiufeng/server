#include "src/server.h"

dx::Logger::ptr g_logger = SERVER_LOG_ROOT();

void run_in_fiber() {
    SERVER_LOG_INFO(g_logger) << "run_in_fiber begin";
    dx::Fiber::GetThis()->YieldToHold();
    SERVER_LOG_INFO(g_logger) << "run_in_fiber end";
    dx::Fiber::GetThis()->YieldToHold();
}

void test_fiber() {
    dx::Fiber::GetThis();
    SERVER_LOG_INFO(g_logger) << "main begin: ";
    
    dx::Fiber::ptr fiber(new dx::Fiber(run_in_fiber));
    fiber->SwapIn();
    SERVER_LOG_INFO(g_logger) << "main after SwapIn";
    fiber->SwapIn();
    SERVER_LOG_INFO(g_logger) << "main after end";
    fiber->SwapIn();
}

int main(int argc, char** argv) {
    dx::Thread::SetNameS("main");
    
    std::vector<dx::Thread::ptr> thrs;
    for(int i = 0; i < 3; i++) {
        dx::Thread::ptr th = dx::Thread::ptr(new dx::Thread(test_fiber, "name_" + std::to_string(i)));
        thrs.push_back(th);
    }

    for(int i = 0; i < 3; i++) {
        thrs[i]->Join();
    }
    return 0;
}
