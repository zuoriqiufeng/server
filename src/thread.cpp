/**
 * @file thread.cpp
 * @author Dingx (dingx@info2soft.com)
 * @brief 
 * 
 * @version 0.1
 * @date 2024-09-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
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

    m_sem.Wait();
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

    thread->m_sem.Notify();

    cb();
    return 0;
} 

SSemaphore::SSemaphore(uint32_t cnt) {

    if(sem_init(&m_sem, 0, cnt)) {
        throw std::logic_error("sem_init error");
    }
}

SSemaphore::~SSemaphore() {
    sem_destroy(&m_sem);  
}

void SSemaphore::Wait() {
    if(sem_wait(&m_sem)) {
       throw std::logic_error("sem_wait error"); 
    }
}

void SSemaphore::Notify() {
    if(sem_post(&m_sem)) {
        throw std::logic_error("sem_post error");
    }
}
    

}
