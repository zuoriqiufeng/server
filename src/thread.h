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

template<class T>
class ScopeLockImpl {
public:
    ScopeLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.Lock();
        m_locked = true;
    }

    ~ScopeLockImpl() {
        m_mutex.Unlock(); 
    }

    void Lock() {
        if(!m_locked) {
            m_mutex.Lock();
            m_locked = true;
        }
    }

    void Unlock() {
        if(m_locked) {
            m_mutex.Unlock();
            m_locked = false;
        }
    }   

private:
    T& m_mutex;
    bool m_locked;
};


template<class T>
class ReadScopeLockImpl {
public:
    ReadScopeLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.RdLock();
        m_locked = true;
    }

    ~ReadScopeLockImpl() {
        m_mutex.Unlock(); 
    }

    void Lock() {
        if(!m_locked) {
            m_mutex.RdLock();
            m_locked = true;
        }
    }

    void Unlock() {
        if(m_locked) {
            m_mutex.Unlock();
            m_locked = false;
        }
    }   

private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
class WriteScopeLockImpl {
public:
    WriteScopeLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.WrLock();
        m_locked = true;
    }

    ~WriteScopeLockImpl() {
        m_mutex.Unlock(); 
    }

    void Lock() {
        if(!m_locked) {
            m_mutex.WrLock();
            m_locked = true;
        }
    }

    void Unlock() {
        if(m_locked) {
            m_mutex.Unlock();
            m_locked = false;
        }
    }   

private:
    T& m_mutex;
    bool m_locked;
};

class SMutex {
public:
    typedef ScopeLockImpl<SMutex> MutexGuard;

    SMutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~SMutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void Lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void Unlock() {
        pthread_mutex_unlock(&m_mutex);
    }   

private:
    pthread_mutex_t m_mutex;
};

class NullMutex {
public:
    typedef ScopeLockImpl<NullMutex> MutexGuard;
    NullMutex() {}
    ~NullMutex() {}

    void Lock() {}
    void Unlock(){}
};

class RWMutex {
public:
    typedef ReadScopeLockImpl<RWMutex> ReadLock;
    typedef WriteScopeLockImpl<RWMutex> WriteLock;

    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    void RdLock() {
        pthread_rwlock_rdlock(&m_lock);
    }

    void WrLock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void Unlock() {
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;

};

class NullRWMutex {
public:
    typedef ReadScopeLockImpl<NullRWMutex> ReadLock;
    typedef WriteScopeLockImpl<NullRWMutex> WriteLock;

    NullRWMutex() {}
    ~NullRWMutex() {}

    void RdLock() {}
    void WrLock() {}
    void Unlock() {}
};

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
    Thread& operator=(const Thread&) = delete;

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
