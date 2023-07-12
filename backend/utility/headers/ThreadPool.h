#include <cstdio>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <functional>
#include <condition_variable>
#include <deque>

/**
 * Used for working with multi-threaded tasks more easily
 *
 * @param threadCount indicates the number of threads created by the ThreadPool
 */
class ThreadPool {
public:
    using TFunction = std::function<void()>;

    explicit ThreadPool(size_t threadCount);

    /**
     * Used to push a new task to the queue of the ThreadPool
     *
     * @param task the task to be pushed
     */
    void push_task(const std::function<void()> &task);

    /**
     * Used to terminate the ThreadPool
     *
     * @param wait indicates whether to wait for the tasks to finish or not (default is set to true)
     */
    void terminate(bool wait = true);

    /**
     * Used to get number of threads used by the ThreadPool
     *
     * @return the number of threads used by the ThreadPool
     */
    size_t thread_count() const;

    /**
     * Used to get the number of tasks in the Queue in the ThreadPool
     *
     * @return the number of tasks in in the Queue in the ThreadPool
     */
    size_t queue_size() const;

    /**
     * Used to check whether the ThreadPool was terminated or not
     *
     * @return true if the ThreadPool was terminated and false - otherwise
     */
    bool terminated() const;

private:
    mutable std::mutex mutex_;

    std::vector<std::thread> threads_;
    std::deque<std::function<void()>> tasks_;
    std::atomic<bool> terminated_;

    std::condition_variable empty_;

};