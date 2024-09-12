#include "src/server.h"

dx::Logger::ptr g_logger = SERVER_LOG_ROOT();

int cnt = 0;
dx::RWMutex s_mutex;

void fun1() {
    SERVER_LOG_INFO(g_logger) << "thread_id: " << dx::Thread::GetNameS()
        << " this.name: " << dx::Thread::GetThis()->GetName()
        << " id: " << dx::GetThreadId()
        << " this.id: " << dx::Thread::GetThis()->GetId();
    
    for(int i = 0; i < 1000000; i++) {
        dx::RWMutex::WriteLock lock(s_mutex);
        cnt++;
    }
}

void fun2 () {

}

int main(int argc, char** argv) {
    SERVER_LOG_INFO(g_logger) << "thread test begin";

    std::vector<dx::Thread::ptr> thrs;

    for(int i = 0; i < 5; i++) {
        dx::Thread::ptr thr(new dx::Thread(&fun1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }


    for(int i = 0; i < 5; i++) {
        thrs[i]->Join();
    }

    SERVER_LOG_INFO(g_logger) << "thread test end";
    SERVER_LOG_INFO(g_logger) << "cnt=" << cnt;
    return 0;
}
