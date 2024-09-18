/**
 * @file scheduler.cpp
 * @author Dingx (dingx@info2soft.com)
 * @brief 协程调度模块
 * 
 * @version 0.1
 * @date 2024-09-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "scheduler.h"
#include "log.h"


namespace dx {

static Logger::ptr g_logger = SERVER_LOG_NAME("system");

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

/**
 * @brief Construct a new Scheduler:: Scheduler object
 * 
 * @param  threads 线程数量
 * @param  use_caller 
 * @param  name 协程调度名称
 */
Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) {
    SERVER_ASSERT(threads > 0);

    if(use_caller) {
        dx::Fiber::GetThis();
        --threads;

        SERVER_ASSERT(GetThis() == nullptr);
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::Run, this)));
        Thread::SetNameS(m_name);

        t_fiber = m_rootFiber.get();
        m_rootThd = GetThreadId();
        m_thIds.push_back(m_rootThd);
    } else {
        m_rootThd = -1;
    }
    m_thCnt = threads;
}

/**
 * @brief Destroy the Scheduler:: Scheduler object
 * 
 */
Scheduler::~Scheduler() {
    SERVER_ASSERT(m_stopping);
    if(GetThis() == this) {
        t_scheduler = nullptr;
    }

}

Scheduler* Scheduler::GetThis() {
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber() {
    return t_fiber;
}

void Scheduler::Start() {
    MutexType::MutexGuard g(m_lock);
    if(!m_stopping)
        return;
    
    m_stopping = false;
    SERVER_ASSERT(m_threads.empty());
    m_threads.resize(m_thCnt);
    for(size_t i = 0; i < m_thCnt; i++) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::Run, this), 
            m_name + "-" + std::to_string(i)));
        m_thIds.push_back(m_threads[i]->GetId());
    }

}

/**
 * @brief 
 * 
 */
void Scheduler::Stop() {
    m_aotuStop = false;
    if(m_rootFiber && m_thCnt == 0 
        && (m_rootFiber->GetState() == Fiber::TERM || m_rootFiber->GetState() == Fiber::INIT)) {
        
        SERVER_LOG_INFO(g_logger) << this << "stopped";
        m_stopping = true;
        
        if(Stopping()) 
            return;

        // bool exit_on_this_fiber = false;
        if(m_rootThd != -1) {
            SERVER_ASSERT(GetThis() == this);
        } else {
            SERVER_ASSERT(GetThis() != this);
        }

        m_stopping = true;
        for(size_t i = 0; i < m_thCnt; i++) {
            Tickle();
        }

        if(m_rootFiber) {
            Tickle();
        }

        if(Stopping()) {
            return;
        }

        // if(exit_on_this_fiber) {

        // }
    }
}

void Scheduler::SetThis() {
    t_scheduler = this;
}

/**
 * @brief 运行协程
 * 
 */
void Scheduler::Run() {
    // 设置当前线程的Scheduler
    SetThis();

    // 初始化当前线程的主协程
    if(dx::GetThreadId() != m_rootThd) {
        t_fiber = Fiber::GetThis().get();
    }

    // 创建两个Fiber, 用于不用类型调度，一个直接是Fiber调度，一个是方法调度
    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::Idle, this)));
    Fiber::ptr cb_fiber;

    FiberAndThread ft;
    while(true) {
        ft.Reset();
        bool tickle_me = false;
        {
            MutexType::MutexGuard g(m_lock);
            auto it = m_fibers.begin();
            while(it != m_fibers.end()) {
                if(it->thread != -1 && it->thread != dx::GetThreadId()) {
                    it++;
                    tickle_me = true;
                    continue;
                }
                SERVER_ASSERT(it->fiber || it->cb);
                if(it->fiber && it->fiber->GetState() == Fiber::EXEC) {
                    it++;
                    continue;
                }

                ft = *it;
                it = m_fibers.erase(it);
            }
        }

        if(tickle_me) {
            Tickle();
        }

        if(ft.fiber && (ft.fiber->GetState() != Fiber::TERM
                        || ft.fiber->GetState() != Fiber::EXCEPT)) {
            ++m_actThdCnt;
            ft.fiber->SwapIn();
            --m_actThdCnt;

            if(ft.fiber->GetState() == Fiber::READY) {
                Schedule(ft.fiber);
            } else if (ft.fiber->GetState() != Fiber::TERM
                        && ft.fiber->GetState() != Fiber::EXCEPT) {
                ft.fiber->SetState(Fiber::HOLD);

            }
            ft.Reset();

        } else if(ft.cb) {

            if(cb_fiber) {
                cb_fiber->Reset(ft.cb);
            } else { 
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.Reset();
            
            ++m_actThdCnt;
            cb_fiber->SwapIn();
            --m_actThdCnt;

            if(cb_fiber->GetState() == Fiber::READY) {
                Schedule(cb_fiber);
                cb_fiber.reset();
            } else if (cb_fiber->GetState() != Fiber::TERM
                        && cb_fiber->GetState() != Fiber::EXCEPT) {
                cb_fiber->Reset(nullptr);

            } else { //if(cb_fiber->GetState() != Fiber::TERM) {
                cb_fiber->SetState( Fiber::HOLD);
                cb_fiber.reset();
            }

        } else {
            // Idle协程结束，直接结束调度
            if(idle_fiber->GetState() == Fiber::TERM)
                break;
            
            m_idleThCnt++;
            idle_fiber->SwapIn();
            if(idle_fiber->GetState() != Fiber::TERM || idle_fiber->GetState())  {
                idle_fiber->SetState(Fiber::HOLD);
            }
            --m_idleThCnt;
        }
    }  

}

bool Scheduler::Stopping() {
    MutexType::MutexGuard g(m_lock);
    return m_aotuStop && m_stopping && m_fibers.empty() && m_actThdCnt == 0;
}

void Scheduler::Tickle() {

}

void Scheduler::Idle() {

}


}
