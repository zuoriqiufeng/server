#ifndef __SERVER_MUTEX_H__
#define __SERVER_MUTEX_H__

#include <mutex>
#include <atomic>

namespace dx {

    
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

class SpinLock {
public:
    typedef ScopeLockImpl<SpinLock> MutexGuard;

    SpinLock() {
        pthread_spin_init(&m_mutex, PTHREAD_PROCESS_PRIVATE);
    }

    ~SpinLock() {
        pthread_spin_destroy(&m_mutex);
    }

    void Lock() {
        pthread_spin_lock(&m_mutex);
    }

    void Unlock() {
        pthread_spin_unlock(&m_mutex);
    }

private:
    pthread_spinlock_t m_mutex;

};

class CASLock {
public:
    typedef ScopeLockImpl<CASLock> MutexGuard;
    CASLock() : m_mutex(ATOMIC_FLAG_INIT) {}

    ~CASLock() {}

    void Lock() {
        while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void Unlock(){
        m_mutex.clear(std::memory_order_release);
    }
private:
    volatile std::atomic_flag m_mutex;
};



}


#endif
