#include "ThreadPool.h"


ThreadPool::ThreadPool(size_t threadCount) : terminated_(false) {
    for (size_t i = 0; i < threadCount; ++i) {
        threads_.emplace_back([&terminated_ = terminated_, &mutex_ = mutex_, &empty_ = empty_, &tasks_ = tasks_] {
            while (!terminated_.load()) {
                bool taskExists = false;
                std::function<void()> func;
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    empty_.wait(lock, [&tasks_ = tasks_, &terminated_ = terminated_] {
                        return tasks_.empty() || terminated_.load();
                    });
                    if (!tasks_.empty()) {
                        taskExists = true;
                        func = tasks_.front();
                        tasks_.pop_front();
                    }
                }
                if (taskExists) {
                    func();
                }
            }
        });
    }
}

void ThreadPool::push_task(const std::function<void()> &task) {
    if (terminated_.load()) {
        throw std::runtime_error("The ThreadPool was terminated and pushing of tasks is not allowed");
    }
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.push_back(task);
    empty_.notify_one();
}

void ThreadPool::terminate(bool wait) {
    terminated_.store(true);
    if (!wait) {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.clear();
    }
    empty_.notify_all();
    for (auto &thread: threads_) {
        thread.join();
    }
}


size_t ThreadPool::thread_count() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return threads_.size();
}

size_t ThreadPool::queue_size() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return tasks_.size();
}

bool ThreadPool::terminated() const {
    return terminated_.load();
}