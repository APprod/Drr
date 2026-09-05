#include "threading/threadImpl.hpp"

MyThreadPool::MyThreadPool(size_t threadCount){
    mylog::GetLogger().Info("Custom thread pool Impl");
    threadNum = std::max(static_cast<size_t>(2), threadCount);
    workers.reserve(threadNum);
    for(size_t i{0}; i < threadNum; i++){
        workers.emplace_back(&worker, this);
    }
}

void MyThreadPool::stop(){
    {
        std::lock_guard lock(mut);
        if (shutdown)
            return;
        shutdown = true;
    }
    m_cv.notify_all();
    for(auto& i : workers){i.join();}
};

void MyThreadPool::worker(){
    while(true){
        std::function<void()> newTask;
        {
            std::unique_lock lock(mut);
            m_cv.wait(lock, [this](){return shutdown || !tasks.empty();});
            if (tasks.empty() && shutdown) break;
            std::swap(newTask, tasks.front());
            tasks.pop();
        }
        newTask();
        {
            std::unique_lock lock(mut);
            --m_taskCount;
            m_allDone.notify_all();
        }
    }
};
