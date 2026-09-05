#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <future>
#include <memory>
#include <condition_variable>
#include <vector>
#include <type_traits>
#include "utils/log.hpp"

class MyThreadPool{
public:
    MyThreadPool(size_t threadCount = std::thread::hardware_concurrency() - 1);
    ~MyThreadPool(){stop();};
    template<typename F, typename ...Args>
    auto enque(F&& func, Args&&... args){
        using ReturnType = std::invoke_result_t<F, Args...>;
        
        auto arguments = std::make_tuple(std::forward<Args>(args)...);

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            [funct = std::forward<F>(func), arg = std::move(arguments)]() mutable{
                return std::apply(std::move(funct), std::move(arg));
            }
        );

        std::future<ReturnType> future = task->get_future();
        {
            std::lock_guard lock(mut);
            tasks.emplace(
                [task]() {
                    (*task)();
                }
            );
            m_taskCount++;
        }
        m_cv.notify_one();
        return future;
    }

    template<typename F, typename ...Args>
    void async(F&& func, Args&&... args){
        auto arguments = std::make_tuple(std::forward<Args>(args)...);
        {
            std::lock_guard lock(mut);
            tasks.emplace(
                [task = std::move(func), arg = std::move(arguments)](){
                    std::apply(task, arg);
                }
            );
            m_taskCount++;
        }
        m_cv.notify_one();
    };
    void stop();
    void waitAll(){
        std::unique_lock lock(mut);
        m_allDone.wait(lock, [this](){return !m_taskCount;});
    }
    
    
private:
    void worker();
    std::vector<std::thread> workers;
    size_t threadNum{1};
    std::queue<std::function<void()>> tasks;
    std::mutex mut;
    std::condition_variable m_cv;

    size_t m_taskCount{0};
    std::condition_variable m_allDone;

    bool shutdown{false};
};
