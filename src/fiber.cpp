#include "fiber.h"
#include <atomic>
#include "config.h"
#include "macro.h"

namespace dx {

static dx::Logger::ptr g_logger = SERVER_LOG_NAME("system");

/**
 * @brief 
 * 
 * 协程id, 协程数量
 * 
 */
static std::atomic<uint64_t> s_fibers_id(0);
static std::atomic<uint64_t> s_fibers_cnt(0);

// 当前协程
static thread_local Fiber* t_fiber = nullptr;

// 当前线程的main协程
static thread_local Fiber::ptr t_thread_fiber = nullptr;

// 协程栈大小
static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

using StatckAllocator = MallocStackAllocator;

/**
 * @brief 把当前线程上下文 变成协程
 *  无参构造只用于创建主协程，所以为私有函数
 * 
 */
Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)) {
        SERVER_ASSERT_ARG(false, "getcontext");
    }
    ++s_fibers_cnt;

    SERVER_LOG_DEBUG(g_logger) << "Fiber::Fiber()";
}

/**
 * @brief 真正创建一个协程
 * 
 * @param  cb
 * @param  stacksize
 */
Fiber::Fiber(std::function<void()> cb, size_t stacksize)
    :m_id(++s_fibers_id),
    m_cb(cb) {
    ++s_fibers_cnt;
    m_stackSize = stacksize ? stacksize : g_fiber_stack_size->GetValue();

    m_stack = StatckAllocator::Alloc(m_stackSize);
    if(getcontext(&m_ctx)) {
        SERVER_ASSERT_ARG(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stackSize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    SERVER_LOG_DEBUG(g_logger) << "Fiber::Fiber() id=" << m_id;
}


Fiber::~Fiber() {
    --s_fibers_cnt;
    if(m_stack) {
        SERVER_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
        StatckAllocator::Free(m_stack, m_stackSize);
    } else {
        SERVER_ASSERT(!m_cb);
        SERVER_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }
    SERVER_LOG_DEBUG(g_logger) << "Fiber::~Fiber() id=" << m_id;
}

/**
 * @brief 重置Fiber函数，并重置状态
 * 
 * @param  cb
 */
void Fiber::Reset(std::function<void()> cb) {
    // 主协程没有栈
    SERVER_ASSERT(m_stack);
    SERVER_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
    
    m_cb = cb;
    if(getcontext(&m_ctx)) {
        SERVER_ASSERT_ARG(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stackSize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

/**
 * @brief 切换到当前协程执行，主协程到后台
 * 
 */
void Fiber::SwapIn() {
    SetThis(this);
    SERVER_ASSERT(m_state != EXEC);

    m_state = EXEC;
    // t_threadFiber = this;
    if(swapcontext(&t_thread_fiber->m_ctx, &m_ctx)) {
        SERVER_ASSERT_ARG(false, "swapcontext");
    }
}

/**
 * @brief 当前协程切换到后台，唤出主协程
 * 
 */
void Fiber::SwapOut() {
    SetThis(t_thread_fiber.get());

    if(swapcontext(&m_ctx, &t_thread_fiber->m_ctx)) {
        SERVER_ASSERT_ARG(false, "swapcontext");
    }
}

/**
 * @brief 设置当前协程
 * 
 * @param  f 协程指针
 */
void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

/**
 * @brief 获取当前协程
 * 
 */
Fiber::ptr Fiber::GetThis() {
    if(t_fiber)
        return t_fiber->shared_from_this();
    Fiber::ptr main_fiber(new Fiber);
    SERVER_ASSERT(t_fiber == main_fiber.get());
    t_thread_fiber = main_fiber;
    return main_fiber;
}

/**
 * @brief 状态转换到Ready
 * 
 */
void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->SwapOut();
}

/**
 * @brief 状态转换到 Hold
 * 
 */
void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    cur->m_state = HOLD;
    cur->SwapOut();
}

/**
 * @brief 返回当前协程总数
 * 
 * @return uint64_t 
 */
uint64_t Fiber::TotalFibers() {
    return s_fibers_cnt;
}

/**
 * @brief 返回FiberId
 * 
 * @return uint64_t 
 */
uint64_t Fiber::GetFiberId() {
    if(t_fiber)
        return t_fiber->GetId();
    return 0;
}

/**
 * @brief 协程主函数
 * 
 */
void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    SERVER_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch(std::exception& ex) {
        cur->m_state = EXCEPT;
        SERVER_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what();        
    } catch(...) {
        cur->m_state = EXCEPT;
        SERVER_LOG_ERROR(g_logger) << "Fiber Except"; 
    }

    // 释放指针
    auto raw_ptr = cur.get();
    cur.reset();
    // SwapOut 回到主协程
    raw_ptr->SwapOut();

    SERVER_ASSERT_ARG(false, "never reach");
}




}
