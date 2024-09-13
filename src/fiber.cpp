#include "fiber.h"
#include <atomic>
#include "config.h"
#include "macro.h"

namespace dx {

/**
 * @brief 
 * 
 * 协程id, 协程数量
 * 
 */
static std::atomic<uint64_t> s_fibers_id(0);
static std::atomic<uint64_t> s_fibers_cnt(0);


static thread_local Fiber* t_fiber = nullptr;
// main协程
static thread_local std::shared_ptr<Fiber::ptr> t_threadFiber = nullptr;

// 协程栈大小
static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32>("fiber.stack_size", 1024 * 1024, "fiber stack size");

using StatckAllocator = MallocStackAllocator;

/**
 * @brief 把当前线程上下文 变成协程
 * 
 */
Fiber::Fiber() {
    m_state = EXEC;

    if(getcontext(&m_ctx)) {
        SERVER_ASSERT_ARG(false, "getcontext");
    }
    ++s_fibers_cnt;
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
}


Fiber::~Fiber() {
    --s_fibers_cnt;
    if(m_stack) {
        SERVER_ASSERT(m_state == TERM || m_state == INIT);
        StatckAllocator::Free(m_stack, m_stackSize);
    } else {
        SERVER_ASSERT(!m_cb);
        SERVER_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }

}

/**
 * @brief 重置Fiber函数，并重置状态
 * 
 * @param  cb
 */
void Fiber::Reset(std::function<void()> cb) {

}

/**
 * @brief 切换到当前协程执行
 * 
 */
void Fiber::SwapIn() {

}

/**
 * @brief 当前协程切换到后台
 * 
 */
void Fiber::SwapOut() {

}

/**
 * @brief 设置当前协程
 * 
 * @param  f
 */
void Fiber::SetThis(Fiber* f) {

}

/**
 * @brief 获取当前协程
 * 
 */
Fiber::ptr Fiber::GetThis() {

}

/**
 * @brief 状态转换到Ready
 * 
 */
void Fiber::YieldToReady() {

}

/**
 * @brief 状态转换到 Hold
 * 
 */
void Fiber::YieldToHold() {

}

/**
 * @brief 返回当前协程总数
 * 
 * @return uint64_t 
 */
uint64_t Fiber::TotalFibers() {

}

/**
 * @brief 协程主函数
 * 
 */
void Fiber::MainFunc() {

}


}
