#ifndef __SERVER_SCHEDULER_H__
#define __SERVER_SCHEDULER_H__
#include <memory>
#include "macro.h"
#include <functional>
#include "thread.h"
#include "mutex.h"
#include <vector>
#include <list>
#include "fiber.h"

namespace dx {

class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef SMutex MutexType;

    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();

    const std::string& GetName() const { return m_name; }
    static Scheduler* GetThis();
    static Fiber* GetMainFiber();

    void Start();
    void Stop();

public:
    template<class FiberOrCb>
    void Schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MutexType::MutexGuard g(m_lock);
            need_tickle = ScheduleNoLock(fc, thread);
        }
        if(need_tickle) {
            Tickle();   
        }
    }
    
    template<class InputIterator>
    void Schedule(InputIterator begin, InputIterator end) {
        bool need_tickle = false;
        {
            MutexType::MutexGuard g(m_lock);
            while(begin != end) {
                need_tickle = ScheduleNoLock(&*begin) || need_tickle;
                begin++;
            }
        }

        if(need_tickle) {
            Tickle();
        }
    }

protected:
    void Run();
    void SetThis();
    virtual bool Stopping();
    virtual void Tickle();
    virtual void Idle();

private:
    template<class FiberOrCb>
    bool ScheduleNoLock(FiberOrCb fc, int thread = -1) {
        bool need_tickle =  m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if(ft.fiber || ft.cb) {
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }

private:
    struct FiberAndThread {
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        FiberAndThread(Fiber::ptr f, int thr) : fiber(f), thread(thr) {}
        FiberAndThread(Fiber::ptr* f, int thr) : thread(thr) {
            fiber.swap(*f);
        }

        FiberAndThread(std::function<void()>  new_cb, int thr) : cb(new_cb), thread(thr) {}
        FiberAndThread(std::function<void()>* new_cb, int thr) : thread(thr) {
            cb.swap(*new_cb);
        }

        FiberAndThread() : thread(-1) {}

        void Reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

private:
    MutexType m_lock;
    std::string m_name;
    std::list<FiberAndThread> m_fibers;
    std::vector<Thread::ptr> m_threads;
    Fiber::ptr m_rootFiber;

protected:
    std::vector<int> m_thIds;
    size_t m_thCnt = 0;
    std::atomic<size_t> m_actThdCnt = {0};
    std::atomic<size_t> m_idleThCnt = {0};
    bool m_stopping = true;
    bool m_aotuStop = false;
    int m_rootThd = 0;
};



}

#endif
