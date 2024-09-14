/**
 * @file thread.h
 * @author Dingx (dingx@info2soft.com)
 * @brief 
 * c11 之前使用 pthread_t
 * c11 之后使用 std::thread, 底层仍然使用pthread_t 实现
 * 
 * 
 * @version 0.1
 * @date 2024-09-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __SERVER_THREAD_H__
#define __SERVER_THREAD_H__

#include <thread>
#include <memory>
#include <functional>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <atomic>
#include "mutex.h"

namespace dx {

class SSemaphore {
public:
    SSemaphore(uint32_t cnt = 0);
    ~SSemaphore();

    void Wait();
    void Notify();

private:
    SSemaphore(const SSemaphore &) = delete;
    SSemaphore(const SSemaphore &&) = delete;
    SSemaphore& operator=(SSemaphore &) = delete; 

private:
    sem_t m_sem;
};

class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;

    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    const std::string& GetName() const { return m_name; }
    pid_t GetId() const { return m_id; }

    /**
     * @brief 线程中止
     * 
     */
    void Join();

    static Thread* GetThis();
    static const std::string& GetNameS();
    static void SetNameS(const std::string& name);


private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;

    /**
     * @brief 线程运行函数
     * 
     * @param  arg 运行参数
     * @return void*
     */
    static void* Run(void *arg);
private:
    pid_t m_id;
    pthread_t m_thread;
    std::function<void()> m_cb;
    std::string m_name;

    SSemaphore m_sem;
};



}

#endif
