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
    while(true) {
        SERVER_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3() {
    while(true) {
        SERVER_LOG_INFO(g_logger) << "====================================";
    }
}

int main(int argc, char** argv) {
    SERVER_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node node = YAML::LoadFile("/home/dx/workspace/server/conf/log2.yaml");
    dx::Config::LoarFromYaml(node);

    std::vector<dx::Thread::ptr> thrs;

    for(int i = 0; i < 2; i++) {
        dx::Thread::ptr thr(new dx::Thread(&fun3, "name_" + std::to_string(i * 2)));
        dx::Thread::ptr thr2(new dx::Thread(&fun2, "name_" + std::to_string(i * 2 + 1)));

        thrs.push_back(thr);
        thrs.push_back(thr2);
    }


    for(size_t i = 0; i < thrs.size(); i++) {
        thrs[i]->Join();
    }

    SERVER_LOG_INFO(g_logger) << "thread test end";
    SERVER_LOG_INFO(g_logger) << "cnt=" << cnt;
    return 0;
}
