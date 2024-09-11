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

namespace dx {

class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;

    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    const std::string& GetName() const { return m_name; }
    pid_t GetId() const { return m_id; }

    void Join();

    static Thread* GetThis();
    static const std::string& GetNameS();
    static void SetNameS(const std::string& name);


private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread operator=(const Thread&) = delete;

    static void* Run(void *arg);
private:
    pid_t m_id;
    pthread_t m_thread;
    std::function<void()> m_cb;
    std::string m_name;
};



}


#endif
