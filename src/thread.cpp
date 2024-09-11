#include "thread.h"
#include "log.h"
#include "util.h"

namespace dx {

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";
static dx::Logger::ptr g_logger = SERVER_LOG_NAME("system");


Thread* Thread::GetThis() {
    return t_thread;
}

const std::string& Thread::GetNameS() {
    return t_thread_name;
}

void Thread::SetNameS(const std::string& name) {
    if(t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}


Thread::Thread(std::function<void()> cb, const std::string& name) 
    :m_cb(cb), m_name(name) {

    if(name.empty()) {
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::Run, this);
    if(rt < 0) {
        SERVER_LOG_ERROR(g_logger) << "ptread_create thread fail, rt = " << rt
            << " name=" << name;
        throw std::logic_error("pthread_create error");
    }
}

Thread::~Thread() {
    if(m_thread) {
        pthread_detach(m_thread);
    }
}

void Thread::Join() {
    if(m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
             SERVER_LOG_ERROR(g_logger) << "ptread_join thread fail, rt = " << rt
            << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

void* Thread::Run(void* arg) {
    Thread* thread = (Thread *) arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = dx::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()>  cb;
    cb.swap(thread->m_cb);

    cb();
    return 0;
} 



}
