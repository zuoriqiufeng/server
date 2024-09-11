#include "src/server.h"

dx::Logger::ptr g_logger = SERVER_LOG_ROOT();


void fun1() {
    SERVER_LOG_INFO(g_logger) << "thread_id: " << dx::Thread::GetNameS()
        << " this.name: " << dx::Thread::GetThis()->GetName()
        << " id: " << dx::GetThreadId()
        << " this.id: " << dx::Thread::GetThis()->GetId();
    
    sleep(20);
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

    return 0;
}
