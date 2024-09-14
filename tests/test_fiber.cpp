#include "src/server.h"

dx::Logger::ptr g_logger = SERVER_LOG_ROOT();

void run_in_fiber() {
    SERVER_LOG_INFO(g_logger) << "run_in_fiber begin";
    dx::Fiber::GetThis()->YieldToHold();
    SERVER_LOG_INFO(g_logger) << "run_in_fiber end";
    dx::Fiber::GetThis()->YieldToHold();
}

int main(int argc, char** argv) {
    dx::Fiber::GetThis();
    SERVER_LOG_INFO(g_logger) << "main begin: ";
    
    dx::Fiber::ptr fiber(new dx::Fiber(run_in_fiber));
    fiber->SwapIn();
    SERVER_LOG_INFO(g_logger) << "main after SwapIn";
    fiber->SwapIn();
    SERVER_LOG_INFO(g_logger) << "main after end";
    fiber->SwapIn();
    return 0;
}
