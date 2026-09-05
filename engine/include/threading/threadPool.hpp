#pragma once
#include <functional>
#include <mutex>
#include <queue>
#ifdef BS_THREAD
#include "BS_thread_pool.hpp"
class ThreadPool{
public:
    
    template<typename F>  
    auto Submit(F&& task){
        return m_pool.submit_task(std::forward<F>(task));
    }
    template<typename F>  
    void Async(F&& task){
        m_pool.detach_task(std::forward<F>(task));
    }
    void WaitAll(){m_pool.wait();}
    void Stop(){m_pool.wait(); m_pool.~thread_pool();}
    static ThreadPool& Get(){
        static ThreadPool sch;
        return sch;
    }
    template<typename F>
    void SubmitToMain(F&& func) {
        std::lock_guard lock{m_queMutex};
        m_mainThreadQueue.emplace_back(std::forward<F>(func));
    }
    void FlushMainQueue() { //Add budget
        std::vector<std::function<void()>> local;
        {
            std::lock_guard lock(m_queMutex);
            std::swap(local, m_mainThreadQueue);
        }
        for (auto& f : local) f();
    }
private:
    BS::thread_pool<BS::tp::none> m_pool;
    ThreadPool(size_t threads = std::thread::hardware_concurrency()):m_pool{threads}{
        m_mainThreadQueue.reserve(threads);
    }
    std::mutex m_queMutex;
    std::vector<std::function<void()>> m_mainThreadQueue;
};
#else
#include "threading/threadImpl.hpp"
class ThreadPool{
public:
    
    template<typename F>  
    auto Submit(F&& task){
        return m_pool.enque(std::forward<F>(task));
    }
    template<typename F>  
    void Async(F&& task){
        m_pool.async(std::forward<F>(task));
    }
    void WaitAll(){;} //Not Implemented
    void Stop(){m_pool.stop(); m_pool.~MyThreadPool();}
    static ThreadPool& Get(){
        static ThreadPool sch;
        return sch;
    }
    template<typename F>
    void SubmitToMain(F&& func) {
        std::lock_guard lock{m_queMutex};
        m_mainThreadQueue.emplace_back(std::forward<F>(func));
    }
    void FlushMainQueue() { //Add budget
        std::vector<std::function<void()>> local;
        {
            std::lock_guard lock(m_queMutex);
            std::swap(local, m_mainThreadQueue);
        }
        for (auto& f : local) f();
    }
private:
    MyThreadPool m_pool;
    ThreadPool(size_t threads = std::thread::hardware_concurrency() - 1):m_pool{threads}{
        m_mainThreadQueue.reserve(threads);
    }
    std::mutex m_queMutex;
    std::vector<std::function<void()>> m_mainThreadQueue;
};
#endif

inline ThreadPool& GetThreadPool(){return ThreadPool::Get();}
