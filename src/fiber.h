#ifndef __SERVER_FIBER_H__
#define __SERVER_FIBER_H__
#include <memory>
#include <ucontext.h>
#include "thread.h"
#include <functional>

namespace dx  {

class Schduler;

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Free(void *vp, size_t size) {
        return free(vp);
    }
};


class Fiber : public std::enable_shared_from_this<Fiber> {
friend class Scheduler;
public:
    typedef std::shared_ptr<Fiber> ptr;
    
    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };
public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();

    void Reset(std::function<void()> cb);
    void SwapIn();
    void SwapOut();
    uint64_t GetId() const { return m_id;};
    int GetState() const { return m_state;}
    void SetState(const Fiber::State state) { m_state = state; }

public:
    static void SetThis(Fiber* f);
    static Fiber::ptr GetThis();
    static void YieldToReady();
    static void YieldToHold();

    static uint64_t TotalFibers();
    static uint64_t GetFiberId();
    static void MainFunc();

private:
    Fiber();

private:
    void*    m_stack = nullptr;
    State    m_state;
    uint64_t m_id;
    uint32_t m_stackSize;
    ucontext_t m_ctx;

    // 真正执行的协程方法
    std::function<void()> m_cb;
    
};






}



#endif
